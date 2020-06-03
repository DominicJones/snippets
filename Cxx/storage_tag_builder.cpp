#include <iostream>
#include <functional>


using StorageTagFn = std::function<std::string (std::string const &)>;

class Variable
{
public:
  Variable(StorageTagFn const storageTagFn) : _storageTagFn(storageTagFn) {}
  StorageTagFn const &getStorageTagFn() const noexcept { return _storageTagFn; }
private:
  StorageTagFn const _storageTagFn;
};


auto differentiableStorageTagFn(std::string const prefix, std::string const &tag)
{
  return prefix + tag;
}


struct DrvMode { enum Option {PRIMAL,TANGENT,ADJOINT}; };

std::string getScopedPrefix(DrvMode::Option mode)
{
  if (mode == DrvMode::TANGENT) return "TangentDrv::";
  if (mode == DrvMode::ADJOINT) return "AdjointDrv::";
  return {};
}

struct DifferentiableVariable : public Variable
{
  DifferentiableVariable(DrvMode::Option mode)
    : Variable(std::bind(&differentiableStorageTagFn, getScopedPrefix(mode), std::placeholders::_1))
  {}
};
