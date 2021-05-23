#ifndef PID_H
#define PID_H
#endif

template <class T>
class PIDController
{
public:
  PIDController(double p, double i, double d);
  void tick(T fbk);
  void setTarget(T t);
  T getTarget();
  T getOutput();
  T getFeedback();
  T getError();
  void setEnabled(bool e);
  bool isEnabled();
  T getProportionalComponent();
  T getIntegralComponent();
  T getDerivativeComponent();
  void setMaxIntegralCumulation(T max);
  T getMaxIntegralCumulation();
  T getIntegralCumulation();

  void setInputBounded(bool bounded);
  bool isInputBounded();
  void setInputBounds(T lower, T upper);
  T getInputLowerBound();
  T getInputUpperBound();
  void setOutputBounded(bool bounded);
  bool isOutputBounded();
  void setOutputBounds(T lower, T upper);
  T getOutputLowerBound();
  T getOutputUpperBound();
  void setFeedbackWrapped(bool wrap);
  bool isFeedbackWrapped();
  void setFeedbackWrapBounds(T lower, T upper);
  T getFeedbackWrapLowerBound();
  T getFeedbackWrapUpperBound();

  void setPID(double p, double i, double d);
  void setP(double p);
  void setI(double i);
  void setD(double d);
  double getP();
  double getI();
  double getD();
  void setPIDSource(T (*pidSource)());
  void setPIDOutput(void (*pidOutput)(T output));
  void registerTimeFunction(unsigned long (*getSystemTime)());
  T output;
private:
  double _p;
  double _i;
  double _d;
  T target;
  bool enabled;
  T currentFeedback;
  T lastFeedback;
  T error;
  T lastError;
  long currentTime;
  long lastTime;
  T integralCumulation;
  T maxCumulation;
  T cycleDerivative;

  bool inputBounded;
  T inputLowerBound;
  T inputUpperBound;
  bool outputBounded;
  T outputLowerBound;
  T outputUpperBound;
  bool feedbackWrapped;
  T feedbackWrapLowerBound;
  T feedbackWrapUpperBound;

  bool timeFunctionRegistered;
  T (*_pidSource)();
  void (*_pidOutput)(T output);
  unsigned long (*_getSystemTime)();
};
