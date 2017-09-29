#pragma once


#include <string>
#include <vector>
#include <unordered_map>
#include <algorithm>
#include <map>
#include <set>



class Table
{
public:
	using Row = std::vector<std::string>;

private:
	Row columns;
	std::unordered_map<std::string, size_t> columnsToIndex;
	std::vector<Row> rows;


public:
	Row& addRow()
	{
		rows.emplace_back(columns.size());
		return rows[rows.size() - 1];
	}


	Row& addRow(const Row& headers, const Row& values)
	{
		if (headers.size() != values.size())
			throw std::runtime_error("Invalid number of values in the row");

		Row& row = addRow();

		for (size_t i = 0; i < headers.size(); i++)
		{
			setRowValue(row, headers[i], values[i]);
		}

		return row;
	}


	void setRowValue(Row& row, const std::string& column, const std::string& value)
	{
		size_t columnIndex = getColumnIndexOrAddColumn(column);
		row[columnIndex] = value;
	}





	size_t getColumnIndexOrFail(const std::string& column) const
	{
		return columnsToIndex.at(column);
	}


	size_t getColumnIndexOrAddColumn(const std::string& column, const std::string& defaultValue = std::string{})
	{
		auto it = columnsToIndex.find(column);

		if (it != columnsToIndex.end())
			return it->second;

		columns.push_back(column);

		size_t index = columns.size() - 1;

		columnsToIndex.emplace(column, index);

		for (Row& row : rows)
			row.emplace_back(defaultValue);

		return index;
	}



	const auto& getRows() const
	{
		return rows;
	}

	const Row& getColumns() const
	{
		return columns;
	}


	void filter(const std::string& column, const std::string& value)
	{
		size_t index = getColumnIndexOrFail(column);

		auto removed = std::remove_if(rows.begin(), rows.end(), [&] (const Row& row) -> bool
		{
			return row[index] != value;
		});
		rows.erase(removed, rows.end());
	}


	Table where(const std::string& column, const std::string& value) const
	{
		Table result = *this;
		result.filter(column, value);
		return result;
	}


	void rename(const std::string& column, const std::string& to)
	{
		size_t index = getColumnIndexOrFail(column);
		columns[index] = to;
		columnsToIndex.erase(column);
		columnsToIndex.emplace(to, index);
	}


	void dropColumn(const std::string& column)
	{
		dropColumn(getColumnIndexOrFail(column));
	}

	void dropColumn(size_t index)
	{
		columnsToIndex.erase(columns[index]);

		columns.erase(columns.begin() + (ptrdiff_t) index);
		for (Row& row : rows)
			row.erase(row.begin() + (ptrdiff_t) index);

		for (size_t i = index; i < columns.size(); i++)
		{
			columnsToIndex[columns[i]] = i;
		}
	}



	void select(std::initializer_list<const std::string> columns)
	{
		size_t i = this->columns.size();
		while (i > 0)
		{
			i--;

			auto it = std::find(columns.begin(), columns.end(), this->columns[i]);
			if (it == columns.end())
				dropColumn(i);
		}
	}



	void joinWith(const Table& table2, const std::string& key)
	{
		auto keyIndex1 = getColumnIndexOrFail(key);
		auto keyIndex2 = getColumnIndexOrFail(key);


		for (const auto& row2 : table2.rows)
		{
			const auto& keyValue2 = row2[keyIndex2];

			auto it = std::find_if(rows.begin(), rows.end(),
				[this, keyIndex1, &keyValue2] (const Row& row1) -> bool
				{
					return row1[keyIndex1] == keyValue2;
				});

			Row* row1;

			if (it == rows.end())
			{
				row1 = &addRow();
				setRowValue(*row1, key, keyValue2);
			}
			else
				row1 = &*it;

			for (size_t i = 0; i < table2.columns.size(); i++)
			{
				if (i != keyIndex2)
				{
					setRowValue(*row1, table2.columns[i], row2[i]);
				}
			}

		}

	}



	Table pivot(const std::string& horizontalCol, const std::string& valueCol, const std::string& key) const
	{
		Table result;

		std::set<std::string> horizontalColValues;

		for (const auto& row : rows)
			horizontalColValues.emplace(row[getColumnIndexOrFail(horizontalCol)]);

		for (const std::string& horizontalColValue : horizontalColValues)
		{
			Table temp = *this;
			temp.filter(horizontalCol, horizontalColValue);
			temp.select({key, valueCol});
			temp.rename(valueCol, horizontalCol + horizontalColValue);

			if (result.getRows().empty())
				result = temp;
			else
				result.joinWith(temp, key);
		}

		return result;
	}


	Table unpivot(const std::string& horizontalColPrefix, const std::string& valueCol, const std::string& parameterName) const
	{
		Table result;

		std::map<size_t, const std::string> horizontalColumnValuesByIndex;

		for (size_t i = 0; i < columns.size(); i++)
		{
			const auto& column = columns[i];
			if (column.compare(0, horizontalColPrefix.size(), horizontalColPrefix) == 0)
				horizontalColumnValuesByIndex.emplace(i, column.substr(horizontalColPrefix.size()));
		}

		for (const auto& horizontalColumnIndexAndValue : horizontalColumnValuesByIndex)
		{
			Table temp = *this;

			auto valueColIndex = temp.getColumnIndexOrAddColumn(valueCol);
			for (Row& row : temp.rows)
				row[valueColIndex] = row[horizontalColumnIndexAndValue.first];



			temp.getColumnIndexOrAddColumn(parameterName, horizontalColumnIndexAndValue.second);

			if (result.columns.empty())
				result = temp;
			else
				result.rows.insert(result.rows.end(), temp.rows.begin(), temp.rows.end());
		}



		return result;
	}
};
