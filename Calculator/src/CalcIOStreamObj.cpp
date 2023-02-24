#pragma once
#include "Common.h"

	/// <summary>
	/// Creates and manages a stream of input calculations as well as values and returns them, 
	/// formatted appropriately via a callback for use with UI elements
	/// Dictates rules and behaviour for input when passed in (only allows certain actions after certain other actions etc..)
	/// </summary>

	void CalcIOStreamObj::ClearOperations()
	{
		// If we've only got 1 value of 0 in the IO stream don't bother clearing
		if (nums[0][0] == 0 && prevActions.size() == 2) { return; }

		// Push our active operations line into our previous operations line
		prevOpString.push_back("--------------\n");
		prevOpString.push_back(activeOpString);
		//prevOpString.push_back("\n");

		// Clear all of our operation lists
		operations.clear();
		nums.clear();
		symbols.clear();
		decimals.clear();
		prevActions.clear();

		// Set our default action
		prevActions = { Action::Start };

		// Reset our calculation value to 0 and reflect it on any linked UI using the AddNum code path
		curVal = 0.0f;
		AddNum(0.0f);	
	}

	void CalcIOStreamObj::DelLast()
	{
		switch (prevActions.back())
		{
			// Reset our value to 0 if we encounter our first action
			case Action::Start:
				AddNum(0.0f);
				return;
			// Delete the last number and if we're left with an empty vector remove it
			case Action::Number:
				if (nums[nums.size() - 1].size() > 0)
				{
					nums[nums.size() - 1].pop_back();
					if (nums[nums.size()-1].size() == 0) { nums.pop_back(); }
				}
				break;
			//Delete the last operation and symbol
			case Action::Operation:
				operations.pop_back();
				symbols.pop_back();
				break;
			// Delete the last decimal and if we're left with an empty vector remove it 
			case Action::Decimal:
				if (decimals[nums.size()-1].size() > 0)
				{
					decimals[nums.size()-1].pop_back();
				}
				else
				{
					decimals.pop_back();
				}
				break;
		}
		// Remove the last action once we've handled it
		prevActions.pop_back();

		// If we remove the last action for some reason reset the stream to 0
		if (prevActions.size() <= 1)
		{
			AddNum(0.0f);
			return;
		}
		// Generate a string to reflect this action
		GenerateStringFromStream();
	}

	void CalcIOStreamObj::Equals()
	{
		switch (prevActions.back())
		{
			// Terminate as an equal is not a valid operation after another operation
			case Action::Operation:
				return;
		}

		// Break here if we have no operations at all in the IO stream as there's nothing to calculate other than the 1st value
		if (operations.size() == 0)
		{
			curVal = nums[0][0];
			GenerateStringFromStream();
			return;
		}

		// Set our current value to the first num/decimal set
		curVal = GetCurrentFloatWithDecimals(0);

		// Set curVal based on all nums, decimals, and operations calculated together
		for (int i = 0; i < operations.size(); i++)
		{
			operations[i](GetCurrentFloatWithDecimals(i+1), curVal);
		}

		// Add our action
		prevActions.push_back(Action::Equal);
		
		// Generate a string to reflect this action
		GenerateStringFromStream();
	}

	float CalcIOStreamObj::GetCurrentFloatWithDecimals(int numIndex)
	{
		// We need to add format all of the relevant num entries into a single number (rather than add them) i.e. 5,5 becomes 55, rather than 10
		std::string s;
		for (int i = 0; i < nums[numIndex].size(); i++)
		{
			s.append(std::to_string(nums[numIndex][i]));
		}
		float f = stof(s);

		// Append decimals to associated numbers if we have any
		if (decimals[numIndex].size() > 0)
		{
			for (int d = 0; d < decimals[numIndex].size(); d++)
			{
				f = f + (decimals[numIndex][d]) / pow(10, d+1);
			}
		}
		return f;
	}

	void CalcIOStreamObj::AddOperation(std::function<void(float amt, float& value)> fnc, char inChar)
	{
		switch (prevActions.back())
		{
			// Overwrite the previous operation and symbol
			case Action::Operation:
				operations.pop_back();
				symbols.pop_back();
				prevActions.pop_back();
				break;
		}
		// Add to our operations, symbols, and previous actions
		operations.push_back(fnc);
		symbols.push_back(inChar);
		prevActions.push_back(Action::Operation);

		// Generate a string to reflect this action
		GenerateStringFromStream();
	}

	void CalcIOStreamObj::AddNum(float inF)
	{
		switch (prevActions.back())
		{
			case Action::Start:
			case Action::Number: 
			{
				// Override last value in the vector nums
				if (nums.size() == 0)
				{
					std::vector<int> num;
					nums.push_back(num);
				}

				// If our only nums entry is 0 replace it with a new number (so we don't start a calculation stream with 01 after pressing 1
				if (nums[nums.size() - 1].size() > 0 && nums[nums.size()-1][0] == 0)
				{
					nums[nums.size() - 1].pop_back();
					nums[nums.size()-1].push_back(inF);

				}
				// Otherwise add a new entry and action
				else
				{
					nums[nums.size() - 1].push_back(inF);
					prevActions.push_back(Action::Number);
				}

				// Try to add a new decimal for this num to make sure everything is enumerated correctly
				TryAddDecimalForNums();
				break;
			}
			// Instantiate a new collection of integers in num as a new operation ends the previous series of nums
			// then add a number + any decimals / actions etc...
			case Action::Operation:
			{
				std::vector<int> num;
				nums.push_back(num);
				prevActions.push_back(Action::Number);
				nums[nums.size()-1].push_back(inF);
				TryAddDecimalForNums();
				break;
			}
			// Add a new decimal to the list associated with the current series of nums
			// Addds associated actions etc..
			case Action::Decimal:
			{
				decimals[nums.size()-1].push_back((int)inF);
				prevActions.push_back(Action::Decimal);
				break;
			}
			// Start a new stream if we're adding a number to an equals, ending the previous calculation stream
			case Action::Equal:
			{
				ClearOperations();
				AddNum(inF);
				return;
			}
		}

		// Generate a string to reflect this action
		GenerateStringFromStream();
	}

	void CalcIOStreamObj::TryAddDecimalForNums()
	{
		// Ensure we have a valid decimals vector for every nums vector that we can add to
		if (decimals.size() < nums.size())
		{
			std::vector<int> dec;
			decimals.push_back(dec);
		}
	}

	void CalcIOStreamObj::SetDecimalMode()
	{
		switch (prevActions.back())
		{
			// return if invalid operation is pressed
			case Action:: Decimal:
			case Action::Equal:
			{
				return;
			}
			// If we can enter decimal mode and the previous action wasn't a number set the number for this decimal stream to 0
			case Action::Start:
			case Action::Operation:
			{
				AddNum(0.0f);
				break;
			}
		}
		// Add to previous actions
		prevActions.push_back(Action::Decimal);
		
		// Generate a string to reflect this action
		GenerateStringFromStream();
	}

	std::string CalcIOStreamObj::CleanFloat(float inF)
	{
		std::string s = std::to_string(inF);
		// Don't bother if our string only has 1 entry
		if (s.size() == 0)
		{
			return s;
		}
		// Iterate backwards removing zeroes till we find a value that'a not zero
		for (int i = s.length() - 1; i--; i > 0)
		{
			if (s[i] == '0') { s.erase(i); }
			else { break; }
		}
		// Remove any decimal places from the float
		if (s[s.length() - 1] == '.')
		{
			s.erase(s.length() - 1);
		}
		return s;
	}

	void CalcIOStreamObj::GenerateStringFromStream()
	{
		activeOpString.clear();
		activeOpString = GenerateActiveOpString();

		switch (prevActions.back())
		{
			// Append the value of our calculation to the active op string, appropriately formatted
			case Action::Equal:
				activeOpString.append("\n=\n");
				activeOpString.append(CleanFloat(curVal));
				break;
		}

		//Callack for the UI formatted using GetOutRef to something the UI can read (current operation string and previous operation string)
		onValUpdated(GetOutRef());
	}

	std::string CalcIOStreamObj::GenerateActiveOpString()
	{
		std::string s = "";
		//Keep track of where we are in each of our collections as we iterate over the IO stream 
		int iNum = 0; int iOp = 0; int iDec = 0;

		// Keep track of our last locally reviewed  action and set the default
		Action locLstAction = Action::Start;
		Action a = locLstAction;

		//Manually iterate here as we want to skip forward an indeces in some cases
		for (int ai = 0; ai < prevActions.size(); ai++)
		{
			switch (a = prevActions[ai])
			{
				// Iterate over all nums in the current nums vector and add them to the string
				case Action::Number:
				{
					if (iNum < nums.size())
					{
						for (int i = 0; i < nums[iNum].size(); i++)
						{
							s.append(CleanFloat(nums[iNum][i]));
							// Manually iterate here as we are going through a nested collection
							// and we need to equate this to a 1 dimensional collection of previous actions
							if (i < nums[iNum].size()-1) { ai++; }
						}
						// Increment our current num index
						iNum++;
						// Reset our current decimal index as we will be dealing with a new set of nums
						iDec = 0;
						//Set last local action
						locLstAction = Action::Number;
					}
					break;
				}
				case Action::Operation:
				{
					// Add the last symbol to the string 
					std::string c(1, symbols[iOp]);
					s.append(c);
					//increment our operations index
					iOp++;
					//Set last local action
					locLstAction = Action::Operation;
					break;
				}
				case Action::Decimal:
				{
					// If we're still iterating over the same vector of decimals continue appending
					if (locLstAction == Action::Decimal) 
					{ 
						s.append(std::to_string(decimals[iNum-1][iDec]));
					}
					// If not assume it's a new stream of decimals and add a decimal place instead
					else { s.append("."); locLstAction = Action::Decimal; break;}
					//increment our decimals index
					iDec++;
					//Set last local action
					locLstAction = Action::Decimal;
					break;
				}
				// We don't need to do anything here other than account for the fact that an equals is part of the previous actions.
				// We handle formatting for equals if it's our last action in GenerateStringFromStream, once this method has been called
				case Action::Equal:
					//Set last local action
					locLstAction = Action::Equal;
					break;
			}
		}
		return s;
	}

	std::tuple<const char*, std::vector<std::string>> CalcIOStreamObj::GetOutRef()
	{
		//Convert 2 std::strings into chars and return them as a tuple for the UI 
		const char* c1 = activeOpString.c_str();
		std::vector<std::string> c2 = prevOpString;
		return std::tuple{ c1, c2 };
	}
