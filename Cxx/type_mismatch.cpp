template<class Value_t>
class StoredVariable
{};


template<int ND, class Component_t>
class CompositeVectorVariable
  : public Component_t
{};


class VelocityComponent
  : public StoredVariable<double>
{};


class Velocity
  : public CompositeVectorVariable<3, VelocityComponent>
{};


class Evaluate
{
public:
  Evaluate(CompositeVectorVariable<3, StoredVariable<double> > const &var) {}
};


int main()
{
  Velocity const var;
  Evaluate const evaluate(var);
}
