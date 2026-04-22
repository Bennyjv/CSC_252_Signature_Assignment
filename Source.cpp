#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <cstdlib>
#include <ctime>

using namespace std;

// Function declarations

int binary_search_recursive(int values[], int searchValue, int startIndex, int endIndex);
int binary_search(int values[], int searchValue, int size);
void createBinaryFile(const string& filename, int count);
void selection_sort(int* values, int size);
void writeBinary(const string& filename, int values[], int size);

// Helper functions for sorting/statistics
void swap_values(int& a, int& b)
{
    int temp = a;
    a = b;
    b = temp;
}

// Binary search functions

int binary_search_recursive(int values[], int searchValue, int startIndex, int endIndex)
{
    if (startIndex > endIndex)
    {
        return -1;
    }

    int middle = startIndex + (endIndex - startIndex) / 2;

    if (values[middle] == searchValue)
    {
        return middle;
    }
    else if (searchValue < values[middle])
    {
        return binary_search_recursive(values, searchValue, startIndex, middle - 1);
    }
    else
    {
        return binary_search_recursive(values, searchValue, middle + 1, endIndex);
    }
}

int binary_search(int values[], int searchValue, int size)
{
    return binary_search_recursive(values, searchValue, 0, size - 1);
}

// Selection sort


void selection_sort(int* values, int size)
{
    for (int i = 0; i < size - 1; i++)
    {
        int minIndex = i;

  for (int j = i + 1; j < size; j++)
   {
            if (values[j] < values[minIndex])
     {
                minIndex = j;
  }
      }

        if (minIndex != i)
{
            swap_values(values[i], values[minIndex]);
  }
    }
}

// Binary file functions


void writeBinary(const string& filename, int values[], int size)
{
    ofstream outFile(filename, ios::binary);

    if (!outFile)
    {
        cerr << "Error opening file for writing: " << filename << endl;
        return;
    }

    outFile.write(reinterpret_cast<char*>(&size), sizeof(size));
    outFile.write(reinterpret_cast<char*>(values), sizeof(int) * size);

    outFile.close();
}

void createBinaryFile(const string& filename, int count)
{
    int* values = new int[count];

    for (int i = 0; i < count; i++)
    {
        values[i] = rand() % 1000; // random values 0 - 999
    }

    writeBinary(filename, values, count);

    delete[] values;
}


// BinaryReader class


class BinaryReader
{
private:
    int* values;
    int size;

public:
    BinaryReader(const string& filename)
    {
        values = nullptr;
        size = 0;

        ifstream inFile(filename, ios::binary);

        if (!inFile)
        {
            cerr << "Error opening file for reading: " << filename << endl;
            return;
        }

        inFile.read(reinterpret_cast<char*>(&size), sizeof(size));

        if (size > 0)
        {
            values = new int[size];
            inFile.read(reinterpret_cast<char*>(values), sizeof(int) * size);
        }

        inFile.close();
    }

    ~BinaryReader()
    {
        delete[] values;
    }

    int* getValues() const
    {
        return values;
    }

    int getSize() const
    {
        return size;
    }
};

// Analyzer base class


class Analyzer
{
protected:
    int* values;
    int size;

    void cloneValues(int sourceValues[], int sourceSize)
    {
        size = sourceSize;
        values = new int[size];

        for (int i = 0; i < size; i++)
        {
            values[i] = sourceValues[i];
        }
    }

public:
    Analyzer(int sourceValues[], int sourceSize)
    {
        values = nullptr;
        size = 0;
        cloneValues(sourceValues, sourceSize);
    }

    virtual ~Analyzer()
    {
        delete[] values;
    }

    virtual string analyze() = 0;
};

// DuplicatesAnalyzer
// Counts duplicate values

class DuplicatesAnalyzer : public Analyzer
{
public:
    DuplicatesAnalyzer(int sourceValues[], int sourceSize)
        : Analyzer(sourceValues, sourceSize)
    {
    }

    string analyze() override
    {
        int duplicateCount = 0;

        for (int i = 0; i < size; i++)
    {
            for (int j = i + 1; j < size; j++)
    {
                if (values[i] == values[j])
    {
                    duplicateCount++;
                    break;
    }
   }
   }

        ostringstream output;
        output << "Duplicate values found: " << duplicateCount;
        return output.str();
    }
};


// MissingAnalyzer
// Counts how many values from 0-999 do not appear

class MissingAnalyzer : public Analyzer
{
public:
    MissingAnalyzer(int sourceValues[], int sourceSize)
        : Analyzer(sourceValues, sourceSize)
    {
    }

    string analyze() override
    {
        int missingCount = 0;

        for (int target = 0; target < 1000; target++)
        {
            bool found = false;

        for (int i = 0; i < size; i++)
    {
          if (values[i] == target)
  {
         found = true;
                    break;
}
    }

       if (!found)
 {
        missingCount++;
  }
    }

        ostringstream output;
        output << "Missing values in range 0-999: " << missingCount;
        return output.str();
    }
};


// SearchAnalyzer
// Sorts first, then searches for 100 random values


class SearchAnalyzer : public Analyzer
{
public:
    SearchAnalyzer(int sourceValues[], int sourceSize)
        : Analyzer(sourceValues, sourceSize)
    {
        selection_sort(values, size);
    }

    string analyze() override
    {
        int foundCount = 0;

        for (int i = 0; i < 100; i++)
        {
            int searchValue = rand() % 1000;

            if (binary_search(values, searchValue, size) != -1)
      {
                foundCount++;
      }
        }

        ostringstream output;
        output << "Found " << foundCount << " out of 100 random search values";
        return output.str();
    }
};


// StatisticsAnalyzer
// Sorts first, then finds min, max, median, mode, average


class StatisticsAnalyzer : public Analyzer
{
public:
    StatisticsAnalyzer(int sourceValues[], int sourceSize)
        : Analyzer(sourceValues, sourceSize)
    {
    }

    string analyze() override
    {
        selection_sort(values, size);

        int minimum = values[0];
        int maximum = values[size - 1];

        double average = 0.0;
        long long sum = 0;

        for (int i = 0; i < size; i++)
        {
            sum += values[i];
        }

        average = static_cast<double>(sum) / size;

        double median = 0.0;
        if (size % 2 == 1)
        {
            median = values[size / 2];
        }
        else
        {
            median = (values[size / 2 - 1] + values[size / 2]) / 2.0;
        }

        int mode = values[0];
        int bestCount = 1;

        int currentValue = values[0];
        int currentCount = 1;

        for (int i = 1; i < size; i++)
        {
            if (values[i] == currentValue)
            {
                currentCount++;
            }
            else
            {
                if (currentCount > bestCount)
        {
                    bestCount = currentCount;
                    mode = currentValue;
        }

                currentValue = values[i];
                currentCount = 1;
        }
        }

        // check last run
        if (currentCount > bestCount)
        {
            bestCount = currentCount;
            mode = currentValue;
        }

        ostringstream output;
        output << "Minimum: " << minimum
            << ", Maximum: " << maximum
            << ", Average: " << average
            << ", Median: " << median
            << ", Mode: " << mode
            << " (appeared " << bestCount << " times)";

        return output.str();
    }
};


int main()
{
    srand(static_cast<unsigned int>(time(0)));

    const string filename = "binary.dat";
    const int DATA_SIZE = 1000;

    createBinaryFile(filename, DATA_SIZE);

    BinaryReader reader(filename);

    int* data = reader.getValues();
    int size = reader.getSize();

    if (data == nullptr || size == 0)
    {
        cerr << "No data was read from file." << endl;
        return 1;
    }

    DuplicatesAnalyzer duplicates(data, size);
    MissingAnalyzer missing(data, size);
    SearchAnalyzer search(data, size);
    StatisticsAnalyzer statistics(data, size);

    cout << duplicates.analyze() << endl;
    cout << missing.analyze() << endl;
    cout << search.analyze() << endl;
    cout << statistics.analyze() << endl;

    return 0;
}
