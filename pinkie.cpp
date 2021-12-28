#include "pinkie.hpp"

namespace SysY::Pinkie {
  int UnitOpr::wop_count(Unit::Type t) {
    switch (t) {
    case Unit::unary:
    case Unit::binary:
    case Unit::copy:
    case Unit::aw:
    case Unit::ar:
    case Unit::calli:
      return 1;
    default:
      return 0;
    }
  }
  int UnitOpr::hwop_count(Unit::Type t) {
    switch (t) {
    case Unit::aw:
    case Unit::calli:
      return 1;
    default:
      return 0;
    }
  }
  int UnitOpr::rop_count(Unit::Type t) {
    switch (t) {
    case Unit::unary:
    case Unit::copy:
    case Unit::jc:
    case Unit::param:
    case Unit::reti:
      return 1;
    case Unit::binary:
    case Unit::aw:
    case Unit::ar:
      return 2;
    default:
      return 0;
    }
  }
  PointerRange<Operand> UnitOpr::ops() {
    return PointerRange{
      _ops - wop_count(type),
      _ops + rop_count(type),
    };
  }
  PointerRange<Operand const> UnitOpr::ops() const {
    return PointerRange{
      _ops - wop_count(type),
      _ops + rop_count(type),
    };
  }
  PointerRange<Operand> UnitOpr::rops() {
    return PointerRange{
      _ops,
      _ops + rop_count(type),
    };
  }
  PointerRange<Operand const> UnitOpr::rops() const {
    return PointerRange{
      _ops,
      _ops + rop_count(type),
    };
  }
  PointerRange<Operand> UnitOpr::hrops() {
    return PointerRange{
      _ops - hwop_count(type),
      _ops + rop_count(type),
    };
  }
  PointerRange<Operand const> UnitOpr::hrops() const {
    return PointerRange{
      _ops - hwop_count(type),
      _ops + rop_count(type),
    };
  }
  PointerRange<Operand> UnitOpr::wops() {
    return PointerRange{
      _ops - wop_count(type),
      _ops,
    };
  }
  PointerRange<Operand const> UnitOpr::wops() const {
    return PointerRange{
      _ops - wop_count(type),
      _ops,
    };
  }

  PointerRange<Operand> Unit::ops() {
    return reinterpret_cast<UnitOpr *>(this)->ops();
  }
  PointerRange<Operand const> Unit::ops() const {
    return reinterpret_cast<UnitOpr const *>(this)->ops();
  }
  PointerRange<Operand> Unit::rops() {
    return reinterpret_cast<UnitOpr *>(this)->rops();
  }
  PointerRange<Operand const> Unit::rops() const {
    return reinterpret_cast<UnitOpr const *>(this)->rops();
  }
  PointerRange<Operand> Unit::hrops() {
    return reinterpret_cast<UnitOpr *>(this)->hrops();
  }
  PointerRange<Operand const> Unit::hrops() const {
    return reinterpret_cast<UnitOpr const *>(this)->hrops();
  }
  PointerRange<Operand> Unit::wops() {
    return reinterpret_cast<UnitOpr *>(this)->wops();
  }
  PointerRange<Operand const> Unit::wops() const {
    return reinterpret_cast<UnitOpr const *>(this)->wops();
  }
} // namespace SysY::Pinkie