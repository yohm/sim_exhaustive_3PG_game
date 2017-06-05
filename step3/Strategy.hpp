//
// Created by Yohsuke Muraes on 2017/05/22.
//

#include <string>
#include <array>
#include <sstream>
#include <cstdint>
#include "Graph.hpp"

#ifndef STEP3_STRATEGY_HPP
#define STEP3_STRATEGY_HPP

enum Action {
  C,
  D
};

char A2C( Action act );

// ShortState: in short notation
//   cc11, cd1-1, dd22 etc...
class ShortState {
public:
  ShortState( Action _a_2, Action _a_1, int8_t _bc_2, int8_t _bc_1 ):
      a_2(_a_2), a_1(_a_1), bc_2(_bc_2), bc_1(_bc_1) {};
  const Action a_2, a_1;
  const int8_t bc_2, bc_1;

  std::string toString() const {
    std::ostringstream oss;
    oss << A2C(a_2) << A2C(a_1) << (int)bc_2 << (int)bc_1;
    return oss.str();
  }
  bool operator==(const ShortState & rhs) const {
    return (a_2==rhs.a_2 && a_1==rhs.a_1 && bc_2==rhs.bc_2 && bc_1==rhs.bc_1);
  }

  static const Action A_STATES[4][2];
  static const int8_t BC_STATES[10][2];
  static const ShortState ALL_STATES[40];

  size_t ID() const {
    for( size_t i=0; i < 40; i++) {
      if( ALL_STATES[i] == *this ) { return i; }
    }
    throw "must not happen";
  }
private:
};

// FullState: state in the standard si notation
//   cccccc, cdcdcd, cddccd etc....
class FullState {
public:
  FullState(Action _a_2, Action _a_1, Action _b_2, Action _b_1, Action _c_2, Action _c_1):
      a_2(_a_2), a_1(_a_1), b_2(_b_2), b_1(_b_1), c_2(_c_2), c_1(_c_1) {};
  FullState(size_t id):
      a_2( ((id>>5)&1)?D:C ), a_1( ((id>>4)&1)?D:C ), b_2( ((id>>3)&1)?D:C ),
      b_1( ((id>>2)&1)?D:C ), c_2( ((id>>1)&1)?D:C ), c_1( ((id>>0)&1)?D:C ) {};
  const Action a_2, a_1, b_2, b_1, c_2, c_1;

  bool operator==(const FullState & rhs) const {
    return (a_2==rhs.a_2 && a_1==rhs.a_1 && b_2==rhs.b_2 && b_1==rhs.b_1 && c_2==rhs.c_2 && c_1&&rhs.c_1);
  }
  int NumDiffInT1(const FullState& rhs) const {
    if( a_2 != rhs.a_2 || b_2 != rhs.b_2 || c_2 != rhs.c_2 ) { return -1; }
    int count = 0;
    if( a_1 != rhs.a_1 ) { count += 1; }
    if( b_1 != rhs.b_1 ) { count += 1; }
    if( c_1 != rhs.c_1 ) { count += 1; }
    return count;
  }

  std::string toString() const {
    std::ostringstream oss;
    oss << A2C(a_2) << A2C(a_1)
        << A2C(b_2) << A2C(b_1)
        << A2C(c_2) << A2C(c_1);
    return oss.str();
  }

  FullState FromB() const { return FullState(b_2, b_1, a_2, a_1, c_2, c_1); } // full state from B's viewpoint
  FullState FromC() const { return FullState(c_2, c_1, a_2, a_1, b_2, b_1); } // full state from C's viewpoint
  ShortState ToShortState() const {
    int8_t bc_2 = 0;
    if( b_2 == D && c_2 == D ) { bc_2 = 2; }
    else if( b_2 == D || c_2 == D ) { bc_2 = 1; }
    else { bc_2 = 0; }

    int8_t bc_1 = 0;
    if( b_1 == D && c_1 == D ) { bc_1 = 2; }
    else if( b_1 == D || c_1 == D ) {
      if( bc_2 == 1 && b_1 == b_2 ) { bc_1 = -1; }
      else { bc_1 = 1; }
    }
    else { bc_1 = 0 ; }
    return ShortState(a_2,a_1,bc_2,bc_1);
  }

  size_t ID() const {  // ID must be 0~63 integer. AllC: 0, AllD: 63
    size_t id = 0;
    if( a_2 == D ) { id += 1 << 5; }
    if( a_1 == D ) { id += 1 << 4; }
    if( b_2 == D ) { id += 1 << 3; }
    if( b_1 == D ) { id += 1 << 2; }
    if( c_2 == D ) { id += 1 << 1; }
    if( c_1 == D ) { id += 1 << 0; }
    return id;
  }
};

class Strategy {
public:
  Strategy( std::array<Action,40> acts ); // construct a strategy from a list of actions
  Strategy( const char acts[40] );
  ~Strategy() {};
  std::array<Action,40> actions;
  std::array<Action,64> fullActions;

  std::string toString() const {
    std::ostringstream oss;
    for( Action a : actions ) {
      oss << A2C(a);
    }
    return oss.str();
  }
  std::string toFullString() const {
    std::ostringstream oss;
    for( Action a : fullActions ) {
      oss << A2C(a);
    }
    return oss.str();
  }
  Action ActionAt( FullState fs ) const { return fullActions[fs.ID()]; }
  Graph TransitionGraph() const;

private:
  void ConstructFullActions();

};

#endif //STEP3_STRATEGY_HPP
