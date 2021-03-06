/*
 * Copyright (C) 2005 National Association of REALTORS(R)
 *
 * All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use, copy,
 * modify, merge, publish, distribute, and/or sell copies of the
 * Software, and to permit persons to whom the Software is furnished
 * to do so, provided that the above copyright notice(s) and this
 * permission notice appear in all copies of the Software and that
 * both the above copyright notice(s) and this permission notice
 * appear in supporting documentation.
 */

#ifndef LIBRETS_TEST_METADATA_ELEMENT_H
#define LIBRETS_TEST_METADATA_ELEMENT_H

#include "librets/MetadataElement.h"

namespace librets {
    
class TestMetadataElement : public MetadataElement
{
  public:
    virtual MetadataType GetType() const { return SYSTEM; }
    std::string GetTypeName() const;
    void SetTypeName(std::string typeName);

    std::string GetId() const;
    
  private:
    std::string mTypeName;
};

typedef boost::shared_ptr<TestMetadataElement> TestMetadataElementPtr;

typedef std::vector<TestMetadataElementPtr> TestMetadataElementList;

}

#endif

/* Local Variables: */
/* mode: c++ */
/* End: */
