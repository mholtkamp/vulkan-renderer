#include "Selector.h"
#include "Text.h"
#include "Input.h"

Selector::Selector() :
	mSelectionIndex(0)
{

}

Selector::~Selector()
{

}

void Selector::Update()
{
	Button::Update();

	// mText should always display the selected string.
	if (mDirty)
	{
		mText->SetText(GetSelectionString());
	}
}

void Selector::OnPressed()
{
	if (IsButtonJustUp(VBUTTON_LEFT))
	{
		Increment();
	}
	else
	{
		Decrement();
	}
	
	Button::OnPressed();
}

void Selector::AddSelection(const std::string & selection)
{
	mSelectionStrings.push_back(selection);
	MarkDirty();
}

void Selector::RemoveSelection(const std::string & selection)
{
	for (int32_t i = int32_t(mSelectionStrings.size()) - 1; i >= 0; ++i)
	{
		if (mSelectionStrings[i] == selection)
		{
			mSelectionStrings.erase(mSelectionStrings.begin() + i);
			MarkDirty();
			break;
		}
	}
}

void Selector::AddSelections(const std::vector<std::string>& selections)
{
	mSelectionStrings = selections;
	MarkDirty();
}

void Selector::RemoveAllSelections()
{
	mSelectionStrings.clear();
	MarkDirty();
}

void Selector::SetSelectionByString(const std::string & string)
{
	for (int32_t i = 0; i < mSelectionStrings.size(); ++i)
	{
		if (mSelectionStrings[i] == string)
		{
			SetSelectionIndex(i);
			break;
		}
	}
}

void Selector::SetSelectionIndex(int32_t index)
{
	mSelectionIndex = index;

	if (mSelectionIndex >= int32_t(mSelectionStrings.size()))
	{
		mSelectionIndex = 0;
	}
	else if (mSelectionIndex < 0)
	{
		mSelectionIndex = int32_t(mSelectionStrings.size()) - 1;
	}

	if (mSelectionStrings.empty())
	{
		mSelectionIndex = 0;
	}

	MarkDirty();
}

void Selector::Increment()
{
	SetSelectionIndex(GetSelectionIndex() + 1);
}

void Selector::Decrement()
{
	SetSelectionIndex(GetSelectionIndex() - 1);
}

const std::string Selector::GetSelectionString() const
{
	std::string retString;

	if (!mSelectionStrings.empty() &&
		mSelectionIndex >= 0 &&
		mSelectionIndex < mSelectionStrings.size())
	{
		retString = mSelectionStrings[mSelectionIndex];
	}

	return retString;
}

int32_t Selector::GetSelectionIndex() const
{
	return mSelectionIndex;
}
