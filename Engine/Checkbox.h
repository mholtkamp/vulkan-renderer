#pragma once

#include "Selector.h"

class CheckBox : public Selector
{
public:

	CheckBox();
	~CheckBox();

	bool IsChecked() const;
	void SetChecked(bool checked);
};