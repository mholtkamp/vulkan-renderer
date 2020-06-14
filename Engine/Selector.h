#pragma once

#include "Button.h"
#include <string>
#include <vector>

class Selector : public Button
{
public:

	Selector();
	~Selector();

	virtual void Update() override;

	virtual void OnPressed() override;

	void AddSelection(const std::string& selection);
	void RemoveSelection(const std::string& selection);
	void AddSelections(const std::vector<std::string>& selections);
	void RemoveAllSelections();

	void SetSelectionByString(const std::string& string);
	void SetSelectionIndex(int32_t index);
	void Increment();
	void Decrement();

	const std::string GetSelectionString() const;
	int32_t GetSelectionIndex() const;

protected:

	std::vector<std::string> mSelectionStrings;
	int32_t mSelectionIndex;
};