// Copyright (C) 2004 by WiRed Team
// Under the GNU General Public License

#ifndef __ACCEL_CENTER_H__
#define __ACCEL_CENTER_H__

#include <math.h>

#define STATE_REVERSE_ACCEL		(0x100)
#define STATE_FORWARD_ACCEL		(0x200)
#define DEFAULT_ACCEL_STEPS		(242)
#define DEFAULT_MAX_ACCEL		(424.0)
#define DEFAULT_INIT_ACCEL		(1.0)
#define STATE_MASK_TYPE			(0xFF)

enum
  {
    ACCEL_TYPE_DEFAULT = 0,
    ACCEL_TYPE_CURSOR,
    ACCEL_TYPE_PATTERN,
    ACCEL_TYPE_USER			/* MUST STAY THE LAST OF THIS ENUM */
  };

typedef struct				s_accel_specs
{
  long					Type;
  long					Steps;
  double				Max;
  double				InitValue;
} t_accel_specs;

class					AccelCenter
{
  long					StateMask;
  long					Step;
  long					Steps;
  double				Max;
  double				Value;
  double				InitValue;
    
 public:
  AccelCenter(const long accel_type, const long steps = 1, const double max_accel = 1, const double init_value = 0);
  ~AccelCenter();
  
  void					SetSteps(long steps) { Steps = steps; }
  void					SetMax(double max) { Max = max ? max : 1; }
  long					GetLValue() { return ((long) floor(Value)); }
  double				GetValue() { return (Value); }
  void					ResetStep() { Step = 0; }
  void					ResetValue() { Value = InitValue; }
  void					Reset();
  void					SetValue(double move);
  void					SetAccelType(long type);
  double				GetNewValue(double move);
  double				ForwardAccel();
  double				ReverseAccel();
};

extern const struct s_accel_specs	AccelSpecs[];


#endif/*__ACCEL_CENTER_H__*/
