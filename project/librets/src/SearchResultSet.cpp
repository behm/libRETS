#include <sstream>
#include "librets/SearchResultSet.h"
#include "librets/ExpatXmlParser.h"
#include "librets/RetsXmlStartElementEvent.h"
#include "librets/RetsXmlEndElementEvent.h"
#include "librets/RetsXmlTextEvent.h"
#include "librets/RetsException.h"
#include "librets/RetsReplyException.h"
#include "librets/util.h"

using namespace librets;
using namespace librets::util;
using std::string;
using std::vector;
using std::istringstream;
using std::ostringstream;
namespace b = boost;

SearchResultSet::SearchResultSet()
{
    mColumns.reset(new StringVector());
}

SearchResultSet::~SearchResultSet()
{
}

void SearchResultSet::Parse(istreamPtr inputStream)
{
    ExpatXmlParserPtr mXmlParser(new ExpatXmlParser(inputStream));
    RetsXmlStartElementEventPtr metadataEvent;
    while (mXmlParser->HasNext())
    {
        RetsXmlEventPtr event = mXmlParser->GetNextSkippingEmptyText();
        RetsXmlStartElementEventPtr startEvent
            = b::dynamic_pointer_cast<RetsXmlStartElementEvent>(event);
        if (!startEvent)
        {
            continue;
        }
        
        string name = startEvent->GetName();
        if (name == "RETS")
        {
            istringstream replyCodeString(
                startEvent->GetAttributeValue("ReplyCode"));
            int replyCode;
            replyCodeString >> replyCode;
            if (replyCode == 20201)
            {
                continue;
            }
            else if (replyCode != 0)
            {
                string meaning = startEvent->GetAttributeValue("ReplyText");
                throw RetsReplyException(replyCode, meaning);
            }
        }
        else if (name == "COUNT")
        {
            istringstream count(startEvent->GetAttributeValue("Records"));
            count >> mCount;
        }
        else if (name == "COLUMNS")
        {
            RetsXmlTextEventPtr textEvent =
                mXmlParser->AssertNextIsTextEvent();
            StringVectorPtr columns = split(textEvent->GetText(), "\t");
            if (columns->size() < 2)
            {
                ostringstream message;
                message << "Unknown column format: " << Output(*columns);
                throw RetsException(message.str());
            }

            for (StringVector::size_type i = 1; i < columns->size() - 1; i++)
            {
                mColumns->push_back(columns->at(i));
                // Need to subtract 1, so it's zero-based
                mColumnToIndex[columns->at(i)] = i - 1;
            }
            mXmlParser->AssertNextIsEndEvent();
        }
        else if (name == "DATA")
        {
            RetsXmlTextEventPtr textEvent =
                mXmlParser->AssertNextIsTextEvent();
            StringVectorPtr data = split(textEvent->GetText(), "\t");
            if (data->size() < 2)
            {
                ostringstream message;
                message << "Unknown data format: " << Output(*data);
                throw RetsException(message.str());
            }

            // Erase the first and last items, which *should* be empty
            data->erase(data->begin());
            data->pop_back();
            mRows.push_back(data);
            mXmlParser->AssertNextIsEndEvent();
        }
    }
    mNextRow = mRows.begin();
    mCurrentRow.reset();
}

bool SearchResultSet::HasNext()
{
    if (mNextRow != mRows.end())
    {
        mCurrentRow = *mNextRow;
        mNextRow++;
        return true;
    }
    else
    {
        mCurrentRow.reset();
        return false;
    }
}

int SearchResultSet::GetCount()
{
    return mCount;
}

StringVectorPtr SearchResultSet::GetColumns()
{
    return mColumns;
}

string SearchResultSet::GetString(int columnIndex)
{
    return mCurrentRow->at(columnIndex);
}

string SearchResultSet::GetString(string columnName)
{
    return GetString(mColumnToIndex[columnName]);
}
