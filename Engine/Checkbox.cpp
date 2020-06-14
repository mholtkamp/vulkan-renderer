#include "Checkbox.h"

Checkbox::Checkbox()
{
	AddSelection("");
	AddSelection("X");

	mTextPaddingRatio.x = 0.25f;
	SetDimensions(24, 24);
}

Checkbox::~Checkbox()
{
}

bool Checkbox::IsChecked() const
{
	return mSelectionIndex != 0;
}

void Checkbox::SetChecked(bool checked)
{
	SetSelectionIndex(checked ? 1 : 0);
}
