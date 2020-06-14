#pragma once

#include "Selector.h"

class Checkbox : public Selector
{
public:

	Checkbox();
	~Checkbox();

	bool IsChecked() const;
	void SetChecked(bool checked);
};