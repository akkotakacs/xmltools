#pragma once

#include "Report.h"
#include <sstream>
#include <vector>
#include <map>

struct XPathResultEntryType {
    std::wstring type;
    std::wstring name;
    std::wstring value;
};

struct XSLTransformResultType {
    std::string data;
    UniMode encoding = UniMode::uniEnd;
};

struct ErrorEntryType {
    long line;           // 0-based line number having the error
    long linepos;        // 0-based position in line
    long filepos;        // 0-based position in whole stream
    std::wstring reason; // error description
};

enum XmlCapabilityType : int {
    GET_ERROR_DETAILS = 1 << 0, // the wrapped api can return errors details
    CHECK_SYNTAX      = 1 << 1, // the wrapped api can perform syntax check
    CHECK_VALIDITY    = 1 << 2, // the wrapped api can perform validity check
    EVALUATE_XPATH    = 1 << 3, // the wrapped api can perform xpath evaluation
    XSL_TRANSFORM     = 1 << 4, // the wrapped api can perform xsl transformation

    ALL_OPTIONS       = GET_ERROR_DETAILS | CHECK_SYNTAX | CHECK_VALIDITY | EVALUATE_XPATH | XSL_TRANSFORM
};

// Option related types
enum class OptionDataType {
    OPTION_TYPE_INTEGER,
    OPTION_TYPE_DOUBLE,
    OPTION_TYPE_BOOLEAN,
    OPTION_TYPE_STRING
};

enum class OptionFormatType {
    OPTION_FORMAT_TEXT,
    OPTION_FORMAT_COMBO,
    OPTION_FORMAT_CHECKBOX
};

struct XmlWrapperOptionType {
    std::wstring label;                                             // the option name or label
    std::wstring description;                                       // a text describing the option
    std::wstring defaultValue;                                      // the option default value
    OptionDataType type = OptionDataType::OPTION_TYPE_STRING;       // the option data type
    OptionFormatType format = OptionFormatType::OPTION_FORMAT_TEXT; // the option format (when displayed in options dialog)
    std::map<std::string, std::string> options;                     // a map with value and caption of combo entries
};

/*
* This abstract class is the interface for external XML-API wrappers.
* The purpose of this interface is to define the expected methods for
* an XML-API wrapper.
*/

class XmlWrapperInterface {
protected:
    std::vector<ErrorEntryType> errors;

    // A map with wrapper options
    std::map<std::string, XmlWrapperOptionType> options;

    void resetErrors() {
        this->errors.clear();
    }

public:
    XmlWrapperInterface() {}
    virtual ~XmlWrapperInterface() {
        this->errors.clear();
    }

    /*
    * Inform about the interface capabilities
    * @return An integer composed of XmlCapabilityType elements
    */
    virtual int getCapabilities() = 0;

    /*
    * Perform syntax check
    * @param xml A pointer on the begin of xml buffer
    * @param size The xml buffer length
    * @return Return true if xml is well formed, otherwise false
    */
    virtual bool checkSyntax(const char* xml, size_t size) = 0;

    /*
    * Perform validity check
    * @param xml A pointer on the begin of xml buffer
    * @param size The xml buffer length
    * @param schemaFilename Optional. A schema file path; required if schema is not declared in root element
    * @param validationNamespace Optional. A validation namespace
    * @return Return true if xml is valid, otherwise false
    */
    virtual bool checkValidity(const char* xml, size_t size, std::wstring schemaFilename = L"", std::wstring validationNamespace = L"") = 0;

    /*
    * Perform xpath evaluation
    * @param xml A pointer on the begin of xml buffer
    * @param size The xml buffer length
    * @param xpath The expression to be evaluated
    * @param ns An optional string describing the required namespaces (ex: "xmlns:npp='http://notepad-plus-plus.org' xmlns:a='another-namespace'")
    * @param A vector containing all evaluation occurrences
    */
    virtual std::vector<XPathResultEntryType> xpathEvaluate(const char* xml, size_t size, std::wstring xpath, std::wstring ns = L"") = 0;

    /*
    * Perform xsl transformation
    * @param xml A pointer on the begin of xml buffer
    * @param xmllen The xml buffer length
    * @param xslfile The path of XSL transformation stylesheet
    * @param out A reference to the output object; the output object is updated with transformation result
    * @param options Parameters passed to transformer
    * @param srcEncoding The encoding of source document
    * @param The transformation result as a XSLTransformResultType object
    * @return The function retruns true if transformation could be done without error; if errors occurred, it returns false
    */
    virtual bool xslTransform(const char* xml, size_t xmllen, std::wstring xslfile, XSLTransformResultType* out, std::wstring options = L"", UniMode srcEncoding = UniMode::uniEnd) = 0;

    /*
    * Get errors of last executed action
    * @return A vector of errors descriptions
    */
    std::vector<ErrorEntryType> getLastErrors() {
        return this->errors;
    }

    /*
    * Get wrapper options
    * @return A vector of wrapper options
    */
    std::map<std::string, XmlWrapperOptionType> getOptions() {
        return this->options;
    }
};