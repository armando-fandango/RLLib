/*
 * OnOffPolicyPredictionTest.cpp
 *
 *  Created on: Oct 26, 2013
 *      Author: sam
 */

#include "OnOffPolicyPredictionTest.h"

OnOffPolicyPredictionTest::OnOffPolicyPredictionTest() :
    lineProblem(new LineProblem), randomWalkProblem(new RandomWalk)
{
}

OnOffPolicyPredictionTest::~OnOffPolicyPredictionTest()
{
  delete lineProblem;
  delete randomWalkProblem;
  for (std::vector<OnPolicyTDFactory*>::iterator iter = onPolicyTDFactoryVector.begin();
      iter != onPolicyTDFactoryVector.end(); ++iter)
  {
    if (*iter)
      delete *iter;
  }
  for (std::vector<OffPolicyTDFactory*>::iterator iter = offPolicyTDFactoryVector.begin();
      iter != offPolicyTDFactoryVector.end(); ++iter)
  {
    if (*iter)
      delete *iter;
  }
}

void OnOffPolicyPredictionTest::testTD(FiniteStateGraph* problem, OnPolicyTDFactory* factory,
    const double& lambda, const int& nbEpisodeMax)
{
  Timer timer;
  timer.start();
  int nbEpisode = 0;
  FSGAgentState* agentState = new FSGAgentState(problem);
  Vector<double>* x_t = new PVector<double>(agentState->dimension());
  Vector<double>* x_tp1 = new PVector<double>(agentState->dimension());
  problem->initialize();
  OnPolicyTD<double>* td = factory->create(problem->gamma(), lambda, agentState->vectorNorm(),
      agentState->dimension());
  const Vector<double>* solution = problem->expectedDiscountedSolution();
  while (FiniteStateGraph::distanceToSolution(solution, td->weights()) > factory->precision())
  {
    FiniteStateGraph::StepData stepData = agentState->step();
    //x_t->set(agentState->project(stepData.v_t()));
    x_tp1->set(agentState->currentFeatureState());
    if (stepData.v_t()->empty())
      td->initialize();
    else
      td->update(x_t, x_tp1, stepData.r_tp1);
    if (stepData.s_tp1->v()->empty())
    {
      ++nbEpisode;
      //double error = distanceToSolution(solution, td->weights());
      //std::cout << "nbEpisode=" << nbEpisode << " error=" << error << std::endl;
      if (nbEpisode > nbEpisodeMax)
        break;
    }
    x_t->set(x_tp1);
    Assert::checkValues(td->weights());
  }
  timer.stop();

  double error = FiniteStateGraph::distanceToSolution(solution, td->weights());
  std::cout << "## nbEpisode=" << nbEpisode << " error=" << error << " elapsedTime(ms)="
      << timer.getElapsedTimeInMilliSec() << std::endl;

  delete agentState;
  delete x_t;
  delete x_tp1;
}

void OnOffPolicyPredictionTest::testOffPolicyGTD(RandomWalk* problem, OffPolicyTDFactory* factory,
    const double& lambda, const int& nbEpisodeMax, const double& targetLeftProbability,
    const double& behaviourLeftProbability)
{
  Timer timer;
  timer.start();
  int nbEpisode = 0;
  FSGAgentState* agentState = new FSGAgentState(problem);
  Vector<double>* phi_t = new PVector<double>(agentState->dimension());
  Vector<double>* phi_tp1 = new PVector<double>(agentState->dimension());
  problem->initialize();
  OffPolicyTD<double>* gtd = factory->newTD(problem->gamma(), lambda, agentState->vectorNorm(),
      agentState->dimension());
  Policy<double>* behaviorPolicy = RandomWalk::newPolicy(problem->getActions(),
      behaviourLeftProbability);
  Policy<double>* targetPolicy = RandomWalk::newPolicy(problem->getActions(),
      targetLeftProbability);
  problem->setPolicy(behaviorPolicy);

  const Vector<double>* solution = problem->expectedDiscountedSolution();
  while (FiniteStateGraph::distanceToSolution(solution, gtd->weights()) > factory->precision())
  {
    FiniteStateGraph::StepData stepData = agentState->step();
    //phi_t->set(agentState->project(stepData.v_t()));
    phi_tp1->set(agentState->currentFeatureState());
    if (stepData.v_t()->empty())
      gtd->initialize();
    else
    {
      double pi_t = targetPolicy->pi(stepData.a_t);
      double b_t = behaviorPolicy->pi(stepData.a_t);
      double rho_t = pi_t / b_t;
      gtd->update(phi_t, phi_tp1, rho_t, stepData.r_tp1, 0.0);
    }
    if (stepData.s_tp1->v()->empty())
    {
      ++nbEpisode;
      //double error = distanceToSolution(solution, td->weights());
      //std::cout << "nbEpisode=" << nbEpisode << " error=" << error << std::endl;
      if (nbEpisode > nbEpisodeMax)
        break;
    }
    phi_t->set(phi_tp1);
    Assert::checkValues(gtd->weights());
  }
  timer.stop();

  double error = FiniteStateGraph::distanceToSolution(solution, gtd->weights());
  std::cout << "## nbEpisode=" << nbEpisode << " error=" << error << " elapsedTime(ms)="
      << timer.getElapsedTimeInMilliSec() << std::endl;

  delete targetPolicy;
  delete phi_t;
  delete phi_tp1;
}

void OnOffPolicyPredictionTest::registerTDFactories()
{
  onPolicyTDFactoryVector.push_back(new TDTest);
  onPolicyTDFactoryVector.push_back(new TDLambdaTest);
  onPolicyTDFactoryVector.push_back(new TDLambdaAlphaBoundTest);
  onPolicyTDFactoryVector.push_back(new TDLambdaTrueTest);
  onPolicyTDFactoryVector.push_back(new GTDLambdaTest);
  offPolicyTDFactoryVector.push_back(new GTDLambdaTest);
}

void OnOffPolicyPredictionTest::clearTDFactories()
{
  for (std::vector<OnPolicyTDFactory*>::iterator iter = onPolicyTDFactoryVector.begin();
      iter != onPolicyTDFactoryVector.end(); ++iter)
    delete *iter;
  for (std::vector<OffPolicyTDFactory*>::iterator iter = offPolicyTDFactoryVector.begin();
      iter != offPolicyTDFactoryVector.end(); ++iter)
    delete *iter;
  onPolicyTDFactoryVector.clear();
  offPolicyTDFactoryVector.clear();

}

void OnOffPolicyPredictionTest::testOnLineProblem()
{
  Probabilistic::srand(0);
  registerTDFactories();

  for (std::vector<OnPolicyTDFactory*>::iterator iter = onPolicyTDFactoryVector.begin();
      iter != onPolicyTDFactoryVector.end(); ++iter)
    testTD(lineProblem, *iter, 0, nbEpisodeMax());

  clearTDFactories();
}

void OnOffPolicyPredictionTest::testOnLineProblemWithLambda()
{
  Probabilistic::srand(0);
  registerTDFactories();

  for (std::vector<OnPolicyTDFactory*>::iterator iter = onPolicyTDFactoryVector.begin();
      iter != onPolicyTDFactoryVector.end(); ++iter)
  {
    OnPolicyTDFactory* factory = *iter;
    for (int i = 0; i < factory->lambdaValues()->dimension(); i++)
      testTD(lineProblem, factory, factory->lambdaValues()->getEntry(i), nbEpisodeMax());
  }

  clearTDFactories();
}

void OnOffPolicyPredictionTest::testOnRandomWalkProblem()
{
  Probabilistic::srand(0);
  registerTDFactories();

  for (std::vector<OnPolicyTDFactory*>::iterator iter = onPolicyTDFactoryVector.begin();
      iter != onPolicyTDFactoryVector.end(); ++iter)
    testTD(randomWalkProblem, *iter, 0, nbEpisodeMax());

  clearTDFactories();

}

void OnOffPolicyPredictionTest::testOnRandomWalkProblemWithLambda()
{
  Probabilistic::srand(0);
  registerTDFactories();

  for (std::vector<OnPolicyTDFactory*>::iterator iter = onPolicyTDFactoryVector.begin();
      iter != onPolicyTDFactoryVector.end(); ++iter)
  {
    OnPolicyTDFactory* factory = *iter;
    for (int i = 0; i < factory->lambdaValues()->dimension(); i++)
      testTD(randomWalkProblem, factory, factory->lambdaValues()->getEntry(i), nbEpisodeMax());
  }

  clearTDFactories();
}

void OnOffPolicyPredictionTest::testOffPolicy()
{
  Probabilistic::srand(0);
  registerTDFactories();

  for (std::vector<OffPolicyTDFactory*>::iterator iter = offPolicyTDFactoryVector.begin();
      iter != offPolicyTDFactoryVector.end(); ++iter)
  {
    OffPolicyTDFactory* factory = *iter;
    testOffPolicyGTD(randomWalkProblem, factory, 0, nbEpisodeMax(), 0.2, 0.5);
    testOffPolicyGTD(randomWalkProblem, factory, 0, nbEpisodeMax(), 0.5, 0.2);
  }

  clearTDFactories();
}

void OnOffPolicyPredictionTest::testOffPolicyWithLambda()
{
  Probabilistic::srand(0);
  registerTDFactories();

  for (std::vector<OffPolicyTDFactory*>::iterator iter = offPolicyTDFactoryVector.begin();
      iter != offPolicyTDFactoryVector.end(); ++iter)
  {
    OffPolicyTDFactory* factory = *iter;
    const Vector<double>* lambdas = factory->lambdaValues();
    for (int i = 0; i < lambdas->dimension(); i++)
    {
      testOffPolicyGTD(randomWalkProblem, factory, lambdas->getEntry(i), nbEpisodeMax(), 0.2, 0.5);
      testOffPolicyGTD(randomWalkProblem, factory, lambdas->getEntry(i), nbEpisodeMax(), 0.5, 0.2);
    }
  }

  clearTDFactories();

}

int OnOffPolicyPredictionTest::nbEpisodeMax() const
{
  return 100000;
}

void OnOffPolicyPredictionTest::run()
{
  testOnLineProblem();
  testOnLineProblemWithLambda();
  testOnRandomWalkProblem();
  testOnRandomWalkProblemWithLambda();
  testOffPolicy();
  testOffPolicyWithLambda();
}

RLLIB_TEST_MAKE(OnOffPolicyPredictionTest)
