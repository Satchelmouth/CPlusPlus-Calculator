class CalcIOStreamObj {

	/// <summary>
	/// Creates and manages a stream of input calculations as well as values and returns them, 
	/// formatted appropriately via a callback for use with UI elements
	/// </summary>

public:
	//Callback whenever the value of the calculation stream changes 
	std::function<void(std::tuple<const char*, std::vector<std::string>>)> onValUpdated;

	// CALCULATOR STREAM MANAGEMENT METHODS --------------------------------------------------------------------------------------------
	// Clear entire calculation stream
	void ClearOperations();

	// Remove the last action from the operation stream
	void DelLast();

	// Get current calculation stream sum
	void Equals();

	// MATHEMATICAL OPERATION METHODS --------------------------------------------------------------------------------------------
	// Add an add operation and a corresponding symbol to the current calculation stream 
	void AddOperation(std::function<void(float, float&)>, char);

	// NUMERICAL METHODS --------------------------------------------------------------------------------------------
	// Add a number to the current calculation stream 
	void AddNum(float);

	// Tries to set our IO stream into decimal mode (AddNum is used to push new decimal values into the stream)
	void SetDecimalMode();

private:

	//Possible calculation stream operations
	const enum Action { Start, Number, Operation, Decimal, Equal};

	//The previous actions from the calculation stream, formatted line by line as entries in a vector
	std::vector<Action> prevActions = {Action::Start};

	// The current numerical value of the calculation
	float curVal;

	// String for all previous operations.
	std::vector<std::string> prevOpString;

	// String for the active operation line
	std::string activeOpString;

	// String for the full calculator output Generated each time an operation is called or a number is added
	//std::string streamOutString;

	// A dynamically sized list of all the previous operations that make up this calulation stream
	std::vector<std::function<void(float, float&)>> operations;

	// A dynamically sized list of all the previous symbols that make up this calulation stream (symbols are the ascii representation of operations)
	std::vector<char> symbols;

	// A dynamically sized list of all the previous numbers that make up this calulation stream. All nums have a corresponding entry in the vector; decimals
	std::vector<std::vector<int>> nums;

	// A dynamically sized list of all the previous numbers after the floating point that make up this calulation stream
	std::vector<std::vector<int>> decimals;

	//Try to add a new decimal vector (if we have less decimal vectors than num vectors, to prevent enumeration errors)
	void TryAddDecimalForNums();

	// Clean up a float and return it as a string without lot's of zeros at the end
	std::string CleanFloat(float);

	//Construct a string to represent our active operations. 
	//All formatting rules for the IO Stream are specified here or in GenerateStringFromStream
	std::string GenerateActiveOpString();

	// Generate a string to display on the calculator UI based on the active operations, and all previous operations 
	//All formatting rules for the IO Stream are specified here or in GenerateActiveOpString
	void GenerateStringFromStream();

	//Returns a reference to the output strings; this calculation stream, and all previous calculation streams, for use by the UI
	std::tuple<const char*,std::vector<std::string>> GetOutRef();

	//Return the sum of the input float with associated decimals
	float GetCurrentFloatWithDecimals(int);
};

