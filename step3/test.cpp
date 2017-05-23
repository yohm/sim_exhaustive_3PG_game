#include <iostream>
#include "strategy.hpp"
#include "Game.hpp"

void test_State() {
  ShortState s( C, D, 1, -1);
  std::cout << "state: " << s.toString() << std::endl;
  std::cout << "  id: " << s.ID() << std::endl;
  std::cout << "restored_from_id: " << ShortState::ALL_STATES[ s.ID() ].toString() << std::endl;

  FullState fs(C,D,D,D,C,C);
  std::cout << "fullState: " << fs.toString() << std::endl;
  std::cout << "  toShort: " << fs.ToShortState().toString() << std::endl;
  std::cout << "  id: " << fs.ID() << std::endl;
  std::cout << "restored_from_id: " << FullState(fs.ID()).toString() << std::endl;
  std::cout << "from B: " << fs.FromB().toString() << std::endl;
  std::cout << "  toShortFromB: " << fs.FromB().ToShortState().toString() << std::endl;
  std::cout << "  id: " << fs.FromB().ID() << std::endl;
  std::cout << "from C: " << fs.FromC().toString() << std::endl;
  std::cout << "  toShortFromC: " << fs.FromC().ToShortState().toString() << std::endl;
  std::cout << "  id: " << fs.FromC().ID() << std::endl;
  std::cout << "restored_from_id: " << FullState(fs.FromC().ID()).toString() << std::endl;


  std::cout << "NumDiff 0=" << fs.NumDiffInT1(fs) << std::endl;
  FullState fs2(C,D,D,C,C,C);
  std::cout << "NumDiff 1=" << fs.NumDiffInT1(fs2) << std::endl;
  FullState fs3(C,D,D,C,C,D);
  std::cout << "NumDiff 2=" << fs.NumDiffInT1(fs3) << std::endl;
  FullState fs4(C,C,D,C,C,D);
  std::cout << "NumDiff 3=" << fs.NumDiffInT1(fs4) << std::endl;
  FullState fs5(D,D,D,D,C,C);
  std::cout << "NumDiff -1=" << fs.NumDiffInT1(fs5) << std::endl;
}

void test_Strategy() {
  const std::array<Action,40> acts = {
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D
  };
  Strategy str(acts);
  std::cout << "strategy :" << str.toString() << std::endl;
  std::cout << "  full actions :" << str.toFullString() << std::endl;
  FullState allC(C,C,C,C,C,C);
  std::cout << " action at:" << allC.ID() << " is " << A2C(str.ActionAt(allC)) << std::endl;
  FullState allD(D,D,D,D,D,D);
  std::cout << " action at:" << allD.ID() << " is " << A2C(str.ActionAt(allD)) << std::endl;
  FullState fs3(C,C,C,C,D,D);
  std::cout << " action at:" << fs3.ID() << " is " << A2C(str.ActionAt(fs3)) << std::endl;

  Strategy str2("ccccddddccccddddccccddddccccddddccccdddd");
  std::cout << "strategy2:" << str2.toString() << std::endl;
}

void test_Game() {
  std::cout << "testing Game" << std::endl;
  const std::array<Action,40> acts = {
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D,
      C,C,C,C,D,D,D,D
  };
  Strategy sa(acts), sb(acts), sc(acts);

  Game g(sa,sb,sc);
  FullState initC(C,C,C,C,C,C);
  FullState updated = g.Update( initC );
  std::cout << "  updated from C: " << updated.toString() << std::endl;

  FullState initD(D,D,D,D,D,D);
  FullState s = g.Update( initD );
  std::cout << "  updated from D: " << s.toString() << std::endl;

  FullState init3(C,D,D,C,D,D);
  FullState s3= g.Update( init3 );
  std::cout << "  updated from init3: " << s3.toString() << std::endl;
}

void test_UMatrix() {
  std::cout << "testing Game" << std::endl;
  const std::array<Action,40> acts = {
      C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,
      C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,
      C,C,C,C,C,C,C,C
  };
  Strategy sa(acts), sb(acts), sc(acts);

  Game g(sa,sb,sc);
  umatrix_t m = {0.0};
  g.MakeUMatrix(0.1, m);

  for( auto v : m ) {
    for( double x : v ) {
      printf("%.3f ", x);
    }
    printf("\n");
  }
}

void print_v(vec64_t v) {
  for( auto x : v ) { printf("%.2f ", x); }
  printf("\n");
}

void test_PayoffVector() {
  std::cout << "testing payoff vector" << std::endl;

  double r = 2.0, c = 1.0;
  vec64_t va, vb, vc;
  Game::MakePayoffVector(r,c,va,vb,vc);

  std::cout << "va: ";
  print_v(va);
  std::cout << "vb: ";
  print_v(vb);
  std::cout << "vc: ";
  print_v(vc);
}

void test_PowerMethod() {
  std::cout << "testing payoff vector" << std::endl;
  const std::array<Action,40> acts = {
      C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,
      C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,C,
      C,C,C,C,C,C,C,C
  };
  /*
  const std::array<Action,40> acts = {
      D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,
      D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,D,
      D,D,D,D,D,D,D,D
  };
   */
  Strategy sa(acts), sb(acts), sc(acts);

  Game g(sa,sb,sc);
  umatrix_t m = {0.0};
  double error = 0.01;
  g.MakeUMatrix(error, m);
  vec64_t v = {1.0,0.0};
  vec64_t out;

  print_v(v);
  Game::_MultiplyAndNomalize(m, v, out);
  print_v(out);

  vec64_t out2 = Game::PowerMethod(m, v, 100);
  print_v(out2);
  vec64_t out3 = Game::PowerMethod(m, v, 1000);
  print_v(out3);

  double r = 2.0, c = 1.0;
  vec64_t va, vb, vc;
  Game::MakePayoffVector(r,c,va,vb,vc);
  std::cout << "fa : " << Game::Dot(out3,va) << std::endl;
  std::cout << "fb : " << Game::Dot(out3,vb) << std::endl;
  std::cout << "fc : " << Game::Dot(out3,vc) << std::endl;

  auto fs = g.AveragePayoffs(error, r, c );
  std::cout << std::get<0>(fs) << ' ' << std::get<1>(fs) << ' ' << std::get<2>(fs) << std::endl;
}

int main() {
  std::cout << "Hello, World!" << std::endl;

  test_State();
  test_Strategy();
  test_Game();
  test_UMatrix();
  test_PayoffVector();
  test_PowerMethod();

  return 0;
}