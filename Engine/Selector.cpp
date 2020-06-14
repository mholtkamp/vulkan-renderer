#include "Selector.h"

Selector::Selector() :
	mSelectionIndex(0)
{

}

Selector::~Selector()
{

}

void Selector::OnPressed()
{
	SetSelectionIndex(GetSelectionIndex() + 1);
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

	if (mSelectionIndex >= mSelectionStrings.size())
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
