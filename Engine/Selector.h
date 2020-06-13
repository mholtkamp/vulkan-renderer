#pragma once

#include "Button.h"
#include <string>
#include <vector>

class Selector : public Button
{
public:

	Selector();
	~Selector();

	virtual void OnPressed() override;

	void SetSelectedString(const std::string& string);
	void SetSelectedIndex(int32_t index);

	const std::string& GetSelectedString() const;
	int32_t GetSelectedIndex() const;

protected:

	std::vector<std::string> mSelectionStrings;
	int32_t mSelectionIndex;
};