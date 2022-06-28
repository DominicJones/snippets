// based on
//   David Miller, http://millermattson.com/dave, http://vimeo.com/19569529

#include <vector>
#include <iostream>
#include <cstdlib>
#include <cassert>
#include <cmath>
#include <fstream>
#include <sstream>


class TrainingData
{
public:
  TrainingData(const std::string filename);
  bool isEof(void) { return m_trainingDataFile.eof(); }
  void getTopology(std::vector<unsigned> &topology);

  // Returns the number of input values read from the file:
  unsigned getNextInputs(std::vector<double> &inputVals);
  unsigned getTargetOutputs(std::vector<double> &targetOutputVals);

private:
  std::ifstream m_trainingDataFile;
};

void TrainingData::getTopology(std::vector<unsigned> &topology)
{
  std::string line;
  std::string label;

  getline(m_trainingDataFile, line);
  std::stringstream ss(line);
  ss >> label;
  if (this->isEof() || label.compare("topology:") != 0) {
    abort();
  }

  while (!ss.eof()) {
    unsigned n;
    ss >> n;
    topology.push_back(n);
  }

  return;
}

TrainingData::TrainingData(const std::string filename)
{
  m_trainingDataFile.open(filename.c_str());
}

unsigned TrainingData::getNextInputs(std::vector<double> &inputVals)
{
  inputVals.clear();

  std::string line;
  getline(m_trainingDataFile, line);
  std::stringstream ss(line);

  std::string label;
  ss>> label;
  if (label.compare("in:") == 0) {
    double oneValue;
    while (ss >> oneValue) {
      inputVals.push_back(oneValue);
    }
  }

  return inputVals.size();
}

unsigned TrainingData::getTargetOutputs(std::vector<double> &targetOutputVals)
{
  targetOutputVals.clear();

  std::string line;
  getline(m_trainingDataFile, line);
  std::stringstream ss(line);

  std::string label;
  ss>> label;
  if (label.compare("out:") == 0) {
    double oneValue;
    while (ss >> oneValue) {
      targetOutputVals.push_back(oneValue);
    }
  }

  return targetOutputVals.size();
}




struct Connection
{
  double weight;
  double deltaWeight;
};

class Neuron;

using Layer = std::vector<Neuron>;




class Neuron
{
public:
  Neuron(unsigned numOutputs, unsigned myIndex);
  void setOutputVal(double val) { m_outputVal = val; }
  double getOutputVal(void) const { return m_outputVal; }
  void feedForward(const Layer &prevLayer);
  void calcOutputGradients(double targetVal);
  void calcHiddenGradients(const Layer &nextLayer);
  void updateInputWeights(Layer &prevLayer);

private:
  static double eta;   // [0.0..1.0] overall net training rate
  static double alpha; // [0.0..n] multiplier of last weight change (momentum)
  static double transferFunction(double x);
  static double transferFunctionDerivative(double x);
  static double randomWeight(void) { return rand() / double(RAND_MAX); }
  double sumDOW(const Layer &nextLayer) const;
  double m_outputVal;
  std::vector<Connection> m_outputWeights;
  unsigned m_myIndex;
  double m_gradient;
};

double Neuron::eta = 0.15;    // overall net learning rate, [0.0..1.0]
double Neuron::alpha = 0.5;   // momentum, multiplier of last deltaWeight, [0.0..1.0]


void Neuron::updateInputWeights(Layer &prevLayer)
{
  // The weights to be updated are in the Connection container
  // in the neurons in the preceding layer

  for (unsigned n = 0; n < prevLayer.size(); ++n) {
    Neuron &neuron = prevLayer[n];
    double oldDeltaWeight = neuron.m_outputWeights[m_myIndex].deltaWeight;

    double newDeltaWeight =
      // Individual input, magnified by the gradient and train rate:
      eta
      * neuron.getOutputVal()
      * m_gradient
      // Also add momentum = a fraction of the previous delta weight;
      + alpha
      * oldDeltaWeight;

    neuron.m_outputWeights[m_myIndex].deltaWeight = newDeltaWeight;
    neuron.m_outputWeights[m_myIndex].weight += newDeltaWeight;
  }
}

double Neuron::sumDOW(const Layer &nextLayer) const
{
  double sum = 0.0;

  // Sum our contributions of the errors at the nodes we feed.
  for (unsigned n = 0; n < nextLayer.size() - 1; ++n) {
    sum += m_outputWeights[n].weight * nextLayer[n].m_gradient;
  }

  return sum;
}

void Neuron::calcHiddenGradients(const Layer &nextLayer)
{
  double dow = sumDOW(nextLayer);
  m_gradient = dow * Neuron::transferFunctionDerivative(m_outputVal);
}

void Neuron::calcOutputGradients(double targetVal)
{
  double delta = targetVal - m_outputVal;
  m_gradient = delta * Neuron::transferFunctionDerivative(m_outputVal);
}

double Neuron::transferFunction(double x)
{
  return std::tanh(x);
}

double Neuron::transferFunctionDerivative(double x)
{
  return 1.0 - std::pow(std::tanh(x), 2);
}

void Neuron::feedForward(const Layer &prevLayer)
{
  double sum = 0.0;

  // Sum the previous layer's outputs (which are our inputs)
  // Include the bias node from the previous layer.
  for (unsigned n = 0; n < prevLayer.size(); ++n) {
    sum += prevLayer[n].getOutputVal() *
      prevLayer[n].m_outputWeights[m_myIndex].weight;
  }

  m_outputVal = Neuron::transferFunction(sum);
}

Neuron::Neuron(unsigned numOutputs, unsigned myIndex)
{
  for (unsigned c = 0; c < numOutputs; ++c) {
    m_outputWeights.push_back(Connection());
    m_outputWeights.back().weight = randomWeight();
  }

  m_myIndex = myIndex;
}




class NeuralNetwork
{
public:
  NeuralNetwork(const std::vector<unsigned> &topology);
  void feedForward(const std::vector<double> &inputVals);
  void backPropagation(const std::vector<double> &targetVals);
  void getResults(std::vector<double> &resultVals) const;
  double getRecentAverageError(void) const { return m_recentAverageError; }

private:
  std::vector<Layer> m_layers; // m_layers[layerNum][neuronNum]
  double m_error;
  double m_recentAverageError;
  static double m_recentAverageSmoothingFactor;
};


double NeuralNetwork::m_recentAverageSmoothingFactor = 100.0; // Number of training samples to average over


void NeuralNetwork::getResults(std::vector<double> &resultVals) const
{
  resultVals.clear();

  for (unsigned n = 0; n < m_layers.back().size() - 1; ++n) {
    resultVals.push_back(m_layers.back()[n].getOutputVal());
  }
}

void NeuralNetwork::backPropagation(const std::vector<double> &targetVals)
{
  // Calculate overall net error (RMS of output neuron errors)
  Layer &outputLayer = m_layers.back();
  m_error = 0.0;

  for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
    double delta = targetVals[n] - outputLayer[n].getOutputVal();
    m_error += delta * delta;
  }
  m_error /= outputLayer.size() - 1; // get average error squared
  m_error = sqrt(m_error); // RMS

  // Implement a recent average measurement
  m_recentAverageError =
    (m_recentAverageError * m_recentAverageSmoothingFactor + m_error)
    / (m_recentAverageSmoothingFactor + 1.0);

  // Calculate output layer gradients
  for (unsigned n = 0; n < outputLayer.size() - 1; ++n) {
    outputLayer[n].calcOutputGradients(targetVals[n]);
  }

  // Calculate hidden layer gradients
  for (unsigned layerNum = m_layers.size() - 2; layerNum > 0; --layerNum) {
    Layer &hiddenLayer = m_layers[layerNum];
    Layer &nextLayer = m_layers[layerNum + 1];

    for (unsigned n = 0; n < hiddenLayer.size(); ++n) {
      hiddenLayer[n].calcHiddenGradients(nextLayer);
    }
  }

  // For all layers from outputs to first hidden layer,
  // update connection weights
  for (unsigned layerNum = m_layers.size() - 1; layerNum > 0; --layerNum) {
    Layer &layer = m_layers[layerNum];
    Layer &prevLayer = m_layers[layerNum - 1];

    for (unsigned n = 0; n < layer.size() - 1; ++n) {
      layer[n].updateInputWeights(prevLayer);
    }
  }
}

void NeuralNetwork::feedForward(const std::vector<double> &inputVals)
{
  assert(inputVals.size() == m_layers[0].size() - 1);

  // Assign (latch) the input values into the input neurons
  for (unsigned i = 0; i < inputVals.size(); ++i) {
    m_layers[0][i].setOutputVal(inputVals[i]);
  }

  // forward propagate
  for (unsigned layerNum = 1; layerNum < m_layers.size(); ++layerNum) {
    Layer &prevLayer = m_layers[layerNum - 1];
    for (unsigned n = 0; n < m_layers[layerNum].size() - 1; ++n) {
      m_layers[layerNum][n].feedForward(prevLayer);
    }
  }
}

NeuralNetwork::NeuralNetwork(const std::vector<unsigned> &topology)
{
  unsigned numLayers = topology.size();
  for (unsigned layerNum = 0; layerNum < numLayers; ++layerNum) {
    m_layers.push_back(Layer());
    unsigned numOutputs = layerNum == topology.size() - 1 ? 0 : topology[layerNum + 1];

    // We have a new layer, now fill it with neurons, and
    // add a bias neuron in each layer.
    for (unsigned neuronNum = 0; neuronNum <= topology[layerNum]; ++neuronNum) {
      m_layers.back().push_back(Neuron(numOutputs, neuronNum));
    }

    // Force the bias node's output to 1.0 (it was the last neuron pushed in this layer):
    m_layers.back().back().setOutputVal(1.0);
  }
}




void showVectorVals(std::string label, std::vector<double> &v)
{
  std::cout << label << " ";
  for (unsigned i = 0; i < v.size(); ++i) {
    std::cout << v[i] << " ";
  }

  std::cout << std::endl;
}


int main()
{
  TrainingData trainingData("./trainingData.txt");

  std::vector<unsigned> topology;
  trainingData.getTopology(topology);

  NeuralNetwork neuralNetwork{topology};

  // train the network
  {
    std::vector<double> inputVals;
    std::vector<double> targetVals;
    int trainingPass = 0;
    int n_iter = 1000;

    while (!trainingData.isEof())
    {
      ++trainingPass;
      std::cout << std::endl << "Pass " << trainingPass << std::endl;

      if (trainingData.getNextInputs(inputVals) != topology[0]) {
        break;
      }

      trainingData.getTargetOutputs(targetVals);
      std::vector<double> resultVals;

      for (int iter = 0; iter != n_iter; ++iter)
      {
        // Get new input data and feed it forward:
        showVectorVals("Input: ", inputVals);
        neuralNetwork.feedForward(inputVals);

        // Collect the net's actual output results:
        neuralNetwork.getResults(resultVals);
        showVectorVals("Output:", resultVals);

        // Train the net what the outputs should have been:
        showVectorVals("Target:", targetVals);
        assert(targetVals.size() == topology.back());
        neuralNetwork.backPropagation(targetVals);

        // Report how well the training is working, average over recent samples:
        std::cout << "NN recent average error: "
                  << neuralNetwork.getRecentAverageError() << std::endl;
      }
    }

    std::cout << std::endl << "Done" << std::endl;
  }

  // application of trained network
  {
    std::vector<double> inputVals{0, 0.2, 0.4, 0.6, 0.8};
    std::vector<double> resultVals{1, 0.833333, 0.714286, 0.625, 0.555556};
    showVectorVals("Input: ", inputVals);
    neuralNetwork.feedForward(inputVals);
    neuralNetwork.getResults(resultVals);
    showVectorVals("Output:", resultVals);
  }
}


/* trainingData.txt
topology: 5 10 5
in:            0.0          0.2          0.4          0.6          0.8
out:           1.0     0.833333     0.714286        0.625     0.555556
*/
