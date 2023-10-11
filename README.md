
# DreamCandies Coding Assignment for Stonebranch

## Spec Summary

Develop code to verify the correctness of the migration system.

### Provided

Three full extraction files (.csv) will be provided daily ("customer.csv", "invoice.csv", and "invoice_item.csv")

### Input

Customer sample file ("customer_sample.csv")

### Output

Three smaller files containing extracted customer data from the full extraction files

## Assumptions

- The input files are not sorted.
- `CUSTOMER_CODE` is a unique primary key for records in the customer file.
- There are no dublicate records in the input files. The originating database should enforce this. If it does not, we need additional schema/key information to be able to enforce uniqueness.

## Design Decisions

- I have left some comments in the code to help explain the flow of logic. The presence of basic comments like these are controversial and are only left here since this is a coding exercise.
- With the sample size being only ~1000 customer codes and the extraction files containing millions of records, it is worth sorting the customer codes to reduce the time needed later on when searching the customer codes for matches. With a sorted vector, we can use a binary search with logatithm time complexity instead of a linear search with linear time complexity.
- The double quotes inclosing record elements are being removed when ingesting the data to fix a bug where what was supposed to be a left double quotation mark was being read in as a right double quotation mark. This was likely an issue with the sample data and could be encountered again in future data sets, so the decision was made to eliminate the potential for this bug in the future by trimming the quotation marks from all data ingested for comparison. If we can assume inputs will always have the proper quotation marks encoded, then we no longer need to trim the quotation marks.

## Possible Improvements

- The spec states that the 1000 customers will be sent in the customer_sample.csv file. This solution was written to account for any amount of input customers. If we know that the input will always be 1000, we could replace the vector of strings with an array of size 1000. I did not choose to do so in this solution so that memory would not be unecessarily allocated in the case where the sample is actually less than 1000 and so that the vector could be expanded past 1000, should the customer send a larger sample.
- Keep a history of past extractions instead of overwritting them.
- Consider verifying that all files match the expected format before beginning operations.
- The `extract_` functions follow a similar structure. If this process needed to be performed for multiple files, this could possibly be implemented as a single function with additional parameters to keep the code more DRY. In the current use case, that additional development effort has been deemed unecessary.
- Generate larger data sets for running tests at-scale.
