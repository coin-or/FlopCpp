#include "SmiDiscreteDistribution.hpp"

SmiDiscreteDistribution::~SmiDiscreteDistribution() {
		for (size_t i=0; i<smiDiscrete_.size(); ++i)
			delete smiDiscrete_[i];
	}
