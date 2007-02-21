// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License Version 2, June 1991

// Copyright (C) 2004-2006 by Wired Team
// Under the GNU General Public License

#include <stdio.h>
#include "AccelCenter.h"

const struct s_accel_specs	AccelSpecs[] =
  {
    { ACCEL_TYPE_DEFAULT,	DEFAULT_ACCEL_STEPS,	DEFAULT_MAX_ACCEL,	DEFAULT_INIT_ACCEL	},
    { ACCEL_TYPE_CURSOR,	642,			242.0,			4.2			},
    { ACCEL_TYPE_PATTERN,	42,			20.42,			8.42			},
  };

AccelCenter::AccelCenter(const long accel_type, const long steps, const double max_accel, const double init_value)
{
  StateMask = accel_type & 0xFF;
  Steps = steps;
  Step = 0;
  Max = max_accel ? max_accel : 1;
  Value = 0;
  InitValue = init_value;
}

AccelCenter::~AccelCenter()
{
}

void				AccelCenter::Reset()
{
  unsigned char			youpi;
  
  if ((youpi = (unsigned char) (StateMask & 0xFF)) == ACCEL_TYPE_USER)
    Value = InitValue;
  else
    if ((youpi == ACCEL_TYPE_DEFAULT) || (youpi == ACCEL_TYPE_CURSOR) || (youpi == ACCEL_TYPE_PATTERN))
      Value = AccelSpecs[youpi].InitValue;
  Step = 0;
}

void				AccelCenter::SetValue(double move)
{
  if (move < 0)
    {
      if (~StateMask & STATE_REVERSE_ACCEL)
	{
	  Step = 0;
	  StateMask |= STATE_REVERSE_ACCEL;
	}
      ReverseAccel();
    }
  else
    {
      if (StateMask & STATE_REVERSE_ACCEL)
	{
	  Step = 0;
	  StateMask &= ~STATE_REVERSE_ACCEL;
	}
      ForwardAccel();
    }
}

void				AccelCenter::SetAccelType(long type)
{
  if ((StateMask & (type & 0xFF)) != type)
    {
      StateMask = (StateMask & ~STATE_MASK_TYPE) | (type & 0xFF);
      Reset();
    }
}

double				AccelCenter::GetNewValue(double move)
{
  SetValue(move);
  return (Value);
}

double				AccelCenter::ForwardAccel()
{
  long				youpi;
  
  if (~StateMask & STATE_FORWARD_ACCEL)
    {
      StateMask |= STATE_FORWARD_ACCEL;
      Reset();
    }
  if ((youpi = StateMask & 0xFF) >= ACCEL_TYPE_USER)
    {
      Value = InitValue + (Step++ * Max) / (double) Steps;
      if (Step > Steps)
	Step = Steps;
    }
  else
    if ((youpi == ACCEL_TYPE_DEFAULT) || (youpi == ACCEL_TYPE_CURSOR) || (youpi == ACCEL_TYPE_PATTERN))
      {
	Value = AccelSpecs[youpi].InitValue + (Step++ * AccelSpecs[youpi].Max) / (double) AccelSpecs[youpi].Steps;
	if (Step > AccelSpecs[youpi].Steps)
	  Step = AccelSpecs[youpi].Steps;
      }
  return (Value);
}

double				AccelCenter::ReverseAccel()
{
  long				youpi;

  if (StateMask & STATE_FORWARD_ACCEL)
    {
      StateMask &= ~STATE_FORWARD_ACCEL;
      Reset();
    }
  if ((youpi = StateMask & 0xFF) == ACCEL_TYPE_USER)
    Value = -InitValue - (Step-- * Max) / (double) Steps;
  else
    if ((youpi == ACCEL_TYPE_DEFAULT) || (youpi == ACCEL_TYPE_CURSOR) || (youpi == ACCEL_TYPE_PATTERN))
      Value = -AccelSpecs[youpi].InitValue + (Step-- * AccelSpecs[youpi].Max) / (double) AccelSpecs[youpi].Steps;
  if (Value < 0)
    Value = 0;
  if (Step < 0)
    Step = 0;
  return (Value);
}

AccelCenter			AccelCenter::operator=(const AccelCenter& right)
{
	if (this != &right)
	{
		StateMask = right.StateMask;
		Step = right.Step;
		Steps = right.Steps;
		Max = right.Max;
		Value = right.Value;
		InitValue = right.InitValue;
	}
	return *this;
}
