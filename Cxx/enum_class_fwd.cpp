// declarations
enum class DrvMode;

void eval(DrvMode const &mode);


// definitions
enum class DrvMode { PRIMAL, };

void eval(DrvMode const &mode) {}


int main()
{
  auto mode = DrvMode::PRIMAL;
  eval(mode);
}
