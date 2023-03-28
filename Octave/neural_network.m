function ccmNN()

clc; clear all; close all

m = 201;
inputLayerSize = 1;
hiddenLayerSize = 20;
outputLayerSize=1;
lambdaReg = 0;

x = linspace(0,1,m);
y = 1./(1+x);

Theta1 = rand(hiddenLayerSize,inputLayerSize+1);
Theta2 = rand(inputLayerSize,hiddenLayerSize+1);
nnParams = [Theta1(:) ; Theta2(:)];

%Conjugate gradients
costFunction = @(p) myNNcost(p, inputLayerSize, hiddenLayerSize, outputLayerSize, x', y', lambdaReg);
options = optimset('MaxIter', 200);
nnParams0 = nnParams; clear nnParams;
[nnParams, cost] = fmincg(costFunction, nnParams0, options);

%Steepest descent
%%step = 10e-0;
%%nIter = 5000;
%%for iSteepestDescent = 1:nIter
%%  [J grad] = myNN(nn_params,1,20,1,x',y',0);
%%  [J max(grad) max(nn_params)]
%%  nn_params = nn_params - step.*grad;
%%end


%Plotting
for isample = 1:m
  X=x(isample);
  yPred(isample) = myNNPred(nnParams, inputLayerSize,hiddenLayerSize,outputLayerSize, X,lambdaReg);
end

plot(x,y,'r','linewidth',2);
hold on; grid on
plot(x,yPred,'b','linewidth',2);
xlabel('X','fontweight','b','fontsize',16)
ylabel('Y','fontweight','b','fontsize',16)
legend('Target','NN')

end

function pred = myNNPred(nn_params, input_layer_size, ...
                hidden_layer_size, num_labels, ...
                x,lambda)

Theta1 = reshape(nn_params(1:hidden_layer_size * (input_layer_size + 1)), ...
                 hidden_layer_size, (input_layer_size + 1));

Theta2 = reshape(nn_params((1 + (hidden_layer_size * (input_layer_size + 1))):end), ...
                 num_labels, (hidden_layer_size + 1));
a1 = sigmoid_eval(Theta1,x);
a2 = sigmoid_eval(Theta2,a1);

pred = a2;
end

function [J grad] = myNNcost(nn_params,input_layer_size, hidden_layer_size, num_labels, X, y, lambda)

Theta1 = reshape(nn_params(1:hidden_layer_size * (input_layer_size + 1)), ...
                 hidden_layer_size, (input_layer_size + 1));

Theta2 = reshape(nn_params((1 + (hidden_layer_size * (input_layer_size + 1))):end), ...
                 num_labels, (hidden_layer_size + 1));

m = size(X, 1);

%%itheta = 5; jtheta = 2;
%%Theta1(itheta,jtheta) = Theta1(itheta,jtheta) + i*1e-31;

J = 0;
Theta1_grad = zeros(size(Theta1));
Theta2_grad = zeros(size(Theta2));
for isample = 1:m
  dL = zeros(1,num_labels);
  x = X(isample,:);
  aa1 = sigmoid_eval(Theta1,x);
  aa2 = sigmoid_eval(Theta2,aa1);
  ysample = y(isample,1:num_labels);
  J = J + (y(isample) - aa2)^2;
  dL = dL + 2*(ysample - aa2);
  %dL = (ysample./aa2);
  dL = -dL/m;

  dtmp = sigmoidGradient_eval(Theta2,aa1) .* dL;
  Theta2_grad = Theta2_grad + (repmat([1 aa1]',1,num_labels).*dtmp)';

   dtmp = (Theta2'*dtmp')';
   dtmp = sigmoidGradient_eval(Theta1,X(isample,:)) .* dtmp(2:end);
   Theta1_grad = Theta1_grad + (repmat([1 X(isample,:)]',1,hidden_layer_size).*dtmp)';
end
J = J / m;

%%real(Theta1_grad(itheta,jtheta))+imag(J)/1e-31

%% Regularization
J = J + lambda/(2*m) * (sum(Theta1(1:end,2:end)(:).^2) + sum(Theta2(1:end,2:end)(:).^2));

Theta1_grad(1:end,2:end) = Theta1_grad(1:end,2:end) + lambda/m .* Theta1(1:end,2:end);
Theta2_grad(1:end,2:end) = Theta2_grad(1:end,2:end) + lambda/m .* Theta2(1:end,2:end);

grad = [Theta1_grad(:) ; Theta2_grad(:)];

end

function aNew = sigmoid_eval(theta,aOld)
  aOld = [ones(size(aOld,1),1) aOld];
  aNew = (theta*aOld')';
  %aNew = sigmoid(aNew);
  aNew = tanh(aNew);
end

function aNewGrad = sigmoidGradient_eval(theta,aOld)
  aOld = [ones(size(aOld,1),1) aOld];
  aNewGrad = (theta*aOld')';
  %aNewGrad = sigmoidGradient(aNewGrad);
  aNewGrad = 1-(tanh(aNewGrad)).^2;
end
