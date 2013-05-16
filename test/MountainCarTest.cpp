/*
 * MountainCarTest.cpp
 *
 *  Created on: May 15, 2013
 *      Author: sam
 */

#include "MountainCarTest.h"

RLLIB_TEST_MAKE(MountainCarTest)

void MountainCarTest::testSarsaTabularActionMountainCar()
{
  srand(time(0));
  cout << "time=" << time(0) << endl;
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderNoHashing<double, float>(1000, 10, true);
  StateToStateAction<double, float>* toStateAction = new TabularAction<double, float>(projector,
      &problem->getDiscreteActionList(), true);
  Trace<double>* e = new RTrace<double>(toStateAction->dimension());

  cout << "|phi_sa|=" << toStateAction->dimension() << endl;
  cout << "||.||=" << toStateAction->vectorNorm() << endl;

  double alpha = 0.15 / toStateAction->vectorNorm();
  double gamma = 0.99;
  double lambda = 0.3;
  Sarsa<double>* sarsa = new Sarsa<double>(alpha, gamma, lambda, e);
  double epsilon = 0.01;
  Policy<double>* acting = new EpsilonGreedy<double>(sarsa, &problem->getDiscreteActionList(),
      epsilon);
  OnPolicyControlLearner<double, float>* control = new SarsaControl<double, float>(acting,
      toStateAction, sarsa);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete e;
  delete sarsa;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testOnPolicyBoltzmannRTraceTabularActionCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;

  Projector<double, float>* projector = new TileCoderHashing<double, float>(1000, 10, false);
  StateToStateAction<double, float>* toStateAction = new TabularAction<double, float>(projector,
      &problem->getDiscreteActionList(), false);

  cout << "|x_t|=" << projector->dimension() << endl;
  cout << "||.||=" << projector->vectorNorm() << endl;
  cout << "|phi_sa|=" << toStateAction->dimension() << endl;
  cout << "||.||=" << toStateAction->vectorNorm() << endl;

  double alpha_v = 0.1 / projector->vectorNorm();
  double alpha_u = 0.01 / projector->vectorNorm();
  double lambda = 0.3;
  double gamma = 0.99;

  Trace<double>* critice = new RTrace<double>(projector->dimension());
  TDLambda<double>* critic = new TDLambda<double>(alpha_v, gamma, lambda, critice);

  PolicyDistribution<double>* acting = new BoltzmannDistribution<double>(toStateAction->dimension(),
      &problem->getDiscreteActionList());

  Trace<double>* actore = new RTrace<double>(toStateAction->dimension());
  Traces<double>* actoreTraces = new Traces<double>();
  actoreTraces->push_back(actore);
  ActorOnPolicy<double, float>* actor = new ActorLambda<double, float>(alpha_u, gamma, lambda,
      acting, actoreTraces);

  OnPolicyControlLearner<double, float>* control = new ActorCritic<double, float>(critic, actor,
      projector, toStateAction);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete critice;
  delete critic;
  delete actore;
  delete actoreTraces;
  delete actor;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testSarsaMountainCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderHashing<double, float>(10000, 10, true);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());
  Trace<double>* e = new RTrace<double>(projector->dimension());
  double alpha = 0.15 / projector->vectorNorm();
  double gamma = 0.99;
  double lambda = 0.3;
  Sarsa<double>* sarsa = new Sarsa<double>(alpha, gamma, lambda, e);
  double epsilon = 0.01;
  Policy<double>* acting = new EpsilonGreedy<double>(sarsa, &problem->getDiscreteActionList(),
      epsilon);
  OnPolicyControlLearner<double, float>* control = new SarsaControl<double, float>(acting,
      toStateAction, sarsa);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete e;
  delete sarsa;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testExpectedSarsaMountainCar()
{
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderHashing<double, float>(10000, 10, true);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());
  Trace<double>* e = new RTrace<double>(projector->dimension());
  double alpha = 0.2 / projector->vectorNorm();
  double gamma = 0.99;
  double lambda = 0.1;
  Sarsa<double>* sarsa = new Sarsa<double>(alpha, gamma, lambda, e);
  double epsilon = 0.01;
  Policy<double>* acting = new EpsilonGreedy<double>(sarsa, &problem->getDiscreteActionList(),
      epsilon);
  OnPolicyControlLearner<double, float>* control = new ExpectedSarsaControl<double, float>(acting,
      toStateAction, sarsa, &problem->getDiscreteActionList());

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(5, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete e;
  delete sarsa;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testGreedyGQOnPolicyMountainCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderHashing<double, float>(10000, 10, true);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());
  Trace<double>* e = new ATrace<double>(projector->dimension());
  double alpha_v = 0.05 / projector->vectorNorm();
  double alpha_w = 0.0 / projector->vectorNorm();
  double gamma_tp1 = 0.9;
  double beta_tp1 = 1.0 - gamma_tp1;
  double lambda_t = 0.1;
  GQ<double>* gq = new GQ<double>(alpha_v, alpha_w, beta_tp1, lambda_t, e);
  //double epsilon = 0.01;
  Policy<double>* acting = new EpsilonGreedy<double>(gq, &problem->getDiscreteActionList(), 0.01);

  OffPolicyControlLearner<double, float>* control = new GQOnPolicyControl<double, float>(acting,
      &problem->getDiscreteActionList(), toStateAction, gq);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete e;
  delete gq;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testGreedyGQMountainCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderHashing<double, float>(1000000, 10, true);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());
  Trace<double>* e = new ATrace<double>(projector->dimension());
  double alpha_v = 0.1 / projector->vectorNorm();
  double alpha_w = .0001 / projector->vectorNorm();
  double gamma_tp1 = 0.99;
  double beta_tp1 = 1.0 - gamma_tp1;
  double lambda_t = 0.4;
  GQ<double>* gq = new GQ<double>(alpha_v, alpha_w, beta_tp1, lambda_t, e);
  //double epsilon = 0.01;
  //Policy<double>* behavior = new EpsilonGreedy<double>(gq,
  //    &problem->getActionList(), epsilon);
  Policy<double>* behavior = new RandomPolicy<double>(&problem->getDiscreteActionList());
  Policy<double>* target = new Greedy<double>(gq, &problem->getDiscreteActionList());
  OffPolicyControlLearner<double, float>* control = new GreedyGQ<double, float>(target, behavior,
      &problem->getDiscreteActionList(), toStateAction, gq);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(10, 5000, 100);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete e;
  delete gq;
  delete behavior;
  delete target;
  delete control;
  delete sim;
}

void MountainCarTest::testOffPACMountainCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderHashing<double, float>(1000000, 10, true);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());

  double alpha_v = 0.05 / projector->vectorNorm();
  double alpha_w = .0001 / projector->vectorNorm();
  double lambda = 0.4;
  double gamma = 0.99;
  Trace<double>* critice = new ATrace<double>(projector->dimension());
  GTDLambda<double>* critic = new GTDLambda<double>(alpha_v, alpha_w, gamma, lambda, critice);
  double alpha_u = 1.0 / projector->vectorNorm();
  PolicyDistribution<double>* target = new BoltzmannDistribution<double>(projector->dimension(),
      &problem->getDiscreteActionList());

  Trace<double>* actore = new ATrace<double>(projector->dimension());
  Traces<double>* actoreTraces = new Traces<double>();
  actoreTraces->push_back(actore);
  ActorOffPolicy<double, float>* actor = new ActorLambdaOffPolicy<double, float>(alpha_u, gamma,
      lambda, target, actoreTraces);

  Policy<double>* behavior = new RandomPolicy<double>(&problem->getDiscreteActionList());

  OffPolicyControlLearner<double, float>* control = new OffPAC<double, float>(behavior, critic,
      actor, toStateAction, projector, gamma);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(10, 5000, 100);
  sim->computeValueFunction();
  control->persist("visualization/mcar_offpac.data");

  control->reset();
  control->resurrect("visualization/mcar_offpac.data");
  sim->test(20, 5000);

  delete problem;
  delete projector;
  delete toStateAction;
  delete critice;
  delete critic;
  delete actore;
  delete actoreTraces;
  delete actor;
  delete behavior;
  delete target;
  delete control;
  delete sim;
}

void MountainCarTest::testOnPolicyContinousActionCar(const int& nbMemory, const double& lambda,
    const double& gamma, double alpha_v, double alpha_u)
{
  srand(time(0));
  Env<float>* problem = new MCar2D;
  Projector<double, float>* projector = new TileCoderHashing<double, float>(nbMemory, 10, false);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getContinuousActionList());

  alpha_v /= projector->vectorNorm();
  alpha_u /= projector->vectorNorm();

  Trace<double>* critice = new RTrace<double>(projector->dimension());
  TDLambda<double>* critic = new TDLambda<double>(alpha_v, gamma, lambda, critice);

  PolicyDistribution<double>* acting = new NormalDistributionScaled<double>(0, 1.0,
      projector->dimension(), &problem->getContinuousActionList());

  Trace<double>* actore1 = new RTrace<double>(projector->dimension());
  Trace<double>* actore2 = new RTrace<double>(projector->dimension());
  Traces<double>* actoreTraces = new Traces<double>();
  actoreTraces->push_back(actore1);
  actoreTraces->push_back(actore2);
  ActorOnPolicy<double, float>* actor = new ActorLambda<double, float>(alpha_u, gamma, lambda,
      acting, actoreTraces);

  OnPolicyControlLearner<double, float>* control = new AverageRewardActorCritic<double, float>(
      critic, actor, projector, toStateAction, 0);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 200);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete critice;
  delete critic;
  delete actore1;
  delete actore2;
  delete actoreTraces;
  delete actor;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testOnPolicyBoltzmannATraceCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;

  Projector<double, float>* projector = new TileCoderHashing<double, float>(10000, 10, false);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());

  double alpha_v = 0.1 / projector->vectorNorm();
  double alpha_u = 0.01 / projector->vectorNorm();
  double lambda = 0.3;
  double gamma = 0.99;

  Trace<double>* critice = new ATrace<double>(projector->dimension());
  TDLambda<double>* critic = new TDLambda<double>(alpha_v, gamma, lambda, critice);

  PolicyDistribution<double>* acting = new BoltzmannDistribution<double>(projector->dimension(),
      &problem->getDiscreteActionList());

  Trace<double>* actore = new ATrace<double>(projector->dimension());
  Traces<double>* actoreTraces = new Traces<double>();
  actoreTraces->push_back(actore);
  ActorOnPolicy<double, float>* actor = new ActorLambda<double, float>(alpha_u, gamma, lambda,
      acting, actoreTraces);

  OnPolicyControlLearner<double, float>* control = new ActorCritic<double, float>(critic, actor,
      projector, toStateAction);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete critice;
  delete critic;
  delete actore;
  delete actoreTraces;
  delete actor;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testOnPolicyBoltzmannRTraceCar()
{
  srand(time(0));
  Env<float>* problem = new MCar2D;

  Projector<double, float>* projector = new TileCoderHashing<double, float>(10000, 10, false);
  StateToStateAction<double, float>* toStateAction = new StateActionTilings<double, float>(
      projector, &problem->getDiscreteActionList());

  double alpha_v = 0.1 / projector->vectorNorm();
  double alpha_u = 0.01 / projector->vectorNorm();
  double lambda = 0.3;
  double gamma = 0.99;

  Trace<double>* critice = new RTrace<double>(projector->dimension());
  TDLambda<double>* critic = new TDLambda<double>(alpha_v, gamma, lambda, critice);

  PolicyDistribution<double>* acting = new BoltzmannDistribution<double>(projector->dimension(),
      &problem->getDiscreteActionList());

  Trace<double>* actore = new RTrace<double>(projector->dimension());
  Traces<double>* actoreTraces = new Traces<double>();
  actoreTraces->push_back(actore);
  ActorOnPolicy<double, float>* actor = new ActorLambda<double, float>(alpha_u, gamma, lambda,
      acting, actoreTraces);

  OnPolicyControlLearner<double, float>* control = new ActorCritic<double, float>(critic, actor,
      projector, toStateAction);

  Simulator<double, float>* sim = new Simulator<double, float>(control, problem);
  sim->run(1, 5000, 300);
  sim->computeValueFunction();

  delete problem;
  delete projector;
  delete toStateAction;
  delete critice;
  delete critic;
  delete actore;
  delete actoreTraces;
  delete actor;
  delete acting;
  delete control;
  delete sim;
}

void MountainCarTest::testOnPolicyContinousActionCar()
{
  testOnPolicyContinousActionCar(10000, 0.4, 0.99, 0.1, 0.001);
}

void MountainCarTest::run()
{
  testSarsaTabularActionMountainCar();
  testOnPolicyBoltzmannRTraceTabularActionCar();
  testSarsaMountainCar();
  testExpectedSarsaMountainCar();
  testGreedyGQOnPolicyMountainCar();
  testGreedyGQMountainCar();
  testOffPACMountainCar();

  testOnPolicyBoltzmannATraceCar();
  testOnPolicyBoltzmannRTraceCar();
  testOnPolicyContinousActionCar();
}
