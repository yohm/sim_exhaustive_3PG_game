//
// Created by Yohsuke Muraes on 2017/05/22.
//

#include <iostream>
#include "Game.hpp"

FullState Game::Update(FullState fs) const {
  Action act_a = sa.ActionAt( fs );
  Action act_b = sb.ActionAt( fs.FromB() );
  Action act_c = sc.ActionAt( fs.FromC() );
  return FullState( fs.a_1, act_a, fs.b_1, act_b, fs.c_1, act_c);
}

void Game::MakeUMatrix( double e, umatrix_t &m) const{
  for( size_t i=0; i<64; i++ ) {
    FullState si(i);
    FullState next = Update(si);
    for( size_t j=0; j<64; j++) {
      FullState sj(j);
      FullState next = Update(sj);
      int d = next.NumDiffInT1(si);
      if( d == -1 ) { m[i][j] = 0.0; }
      else if( d == 3 ) { m[i][j] = e*e*e; }
      else if( d == 2 ) { m[i][j] = e*e*(1.0-e); }
      else if( d == 1 ) { m[i][j] = e*(1.0-e)*(1.0-e); }
      else { m[i][j] = (1.0-e)*(1.0-e)*(1.0-e); }
    }
  }
}

void Game::MakePayoffVector(double r, double cost, vec64_t &va, vec64_t &vb, vec64_t &vc) {
  for( size_t i=0; i<64; i++) {
    FullState fs(i);
    const Action a = fs.a_1, b = fs.b_1, c = fs.c_1;
    size_t num_C = 0;
    if( a == C ) { num_C++;}
    if( b == C ) { num_C++;}
    if( c == C ) { num_C++;}
    double g = num_C * cost * r / 3.0;

    va[i] = ( a == C ) ? g-cost : g;
    vb[i] = ( b == C ) ? g-cost : g;
    vc[i] = ( c == C ) ? g-cost : g;
  }
}

void Game::_MultiplyAndNomalize(const umatrix_t &m, const vec64_t &v, vec64_t &result) {
  double sum = 0.0;
  for( size_t i=0; i<64; i++ ) {
    result[i] = 0.0;
    for( size_t j=0; j<64; j++ ) {
      result[i] += m[i][j] * v[j];
    }
    sum += result[i];
  }
  for( size_t i=0; i<64; i++ ) { result[i] /= sum; }
}

vec64_t Game::PowerMethod(const umatrix_t &m, const vec64_t &init_v, size_t num_iter) {
  vec64_t va = init_v, vb = {0.0};
  vec64_t* in = &va, *out = &vb;
  for( size_t i=0; i<num_iter; i++) {
    _MultiplyAndNomalize(m, *in, *out);
    vec64_t *tmp = in;
    in = out; out = tmp;
  }
  return *out;
}

double Game::Dot(const vec64_t &v1, const vec64_t &v2) {
  double d = 0.0;
  for( size_t i=0; i<v1.size(); i++) {
    d += v1[i] * v2[i];
  }
  return d;
}

double Game::_R2(const vec64_t &v1, const vec64_t &v2) {
  double d = 0.0;
  for( size_t i=0; i<v1.size(); i++) {
    d += (v1[i] - v2[i]) * (v1[i] - v2[i]);
  }
  return d;
}

std::tuple<double, double, double> Game::AveragePayoffs(double error, double multi_f, double cost, size_t num_iter) const {
  umatrix_t m;
  MakeUMatrix(error, m);
  double delta = 0.001;
  vec64_t init = {1.0,0.0};
  vec64_t out = Game::PowerMethod(m, init, num_iter);
  double r2 = _R2(init,out);
  while( r2 > delta ) {
    init = out;
    out = Game::PowerMethod(m, init, num_iter);
    r2 = _R2(init,out);
    // std::cerr << "iterating : " << r2 << std::endl;
  }

  vec64_t va, vb, vc;
  Game::MakePayoffVector(multi_f, cost, va, vb, vc);

  double fa = Game::Dot(out, va);
  double fb = Game::Dot(out, vb);
  double fc = Game::Dot(out, vc);
  return std::tuple<double, double, double>(fa,fb,fc);
}

