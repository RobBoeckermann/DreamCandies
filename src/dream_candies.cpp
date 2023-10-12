#include <gtest/gtest.h>
#include <filesystem>
#include <fstream>

/*!
 * \brief reads customer codes from input file and stores them in a vector
 * \param customer_sample_path path to a .csv containing customer codes
 * \param customer_codes vector to be filled with customer codes from csv
 * \return true if successful
 */
bool set_customer_codes(const std::filesystem::path &customer_sample_path, std::vector<std::string> &customer_codes) {
    std::ifstream customer_sample_csv(customer_sample_path);
    std::string line;

    // verify that the input file begins with the expected header
    getline(customer_sample_csv, line);
    if (line.compare("\"CUSTOMER_CODE\"") != 0) {
        return false;
    }

    // read customer codes from input file, trim quotation marks, and add to vector
    while (getline(customer_sample_csv, line)) {
        customer_codes.push_back(line.substr(1, line.length() - 2));
    }

    // sort the vector and remove any duplicates
    std::sort(customer_codes.begin(), customer_codes.end());
    customer_codes.erase(std::unique(customer_codes.begin(), customer_codes.end()), customer_codes.end());

    return true;
}

/*!
 * \brief reads original customer file and creates new file containing only records corresponding to the provided customer codes
 * \param customer_codes vector filled with customer codes to carry over to new file
 * \return true if successful
 */
bool extract_customers(const std::vector<std::string> &customer_codes) {
    std::ifstream original_customer_csv("original_files/customer.csv");
    std::ofstream extracted_customer_csv("extracted_files/extracted_customer.csv");
    std::string line;
    std::string code;

    // verify expected header
    getline(original_customer_csv, line);
    if (line.compare("\"CUSTOMER_CODE\",\"FIRSTNAME\",\"LASTNAME\"") != 0) {
        return false;
    }
    // write header to new file
    extracted_customer_csv.write(line.c_str(), line.length());
    extracted_customer_csv.put('\n');
    // traverse file and add all records related to the customer codes to the new file
    while (getline(original_customer_csv, line)) {
        std::stringstream record(line);
        if (getline(record, code, ',')) {
            if (binary_search(customer_codes.begin(), customer_codes.end(), code.substr(1, code.length() - 2))) {
                extracted_customer_csv.write(line.c_str(), line.length());
                extracted_customer_csv.put('\n');
            }
        }
    }

    extracted_customer_csv.close();
    return true;
}

/*!
 * \brief reads original invoice item file and creates new file containing only records corresponding to the provided invoice codes
 * \param invoice_codes vector containing invoice codes to carry over to new file
 * \return true if successful
 */
bool extract_invoice_items(std::vector<std::string> &invoice_codes) {
    std::ifstream original_invoice_item_csv("original_files/invoice_item.csv");
    std::ofstream extracted_invoice_item_csv("extracted_files/extracted_invoice_item.csv");
    std::string line;
    std::string code;

    // sort the invoice codes and remove any duplicates
    std::sort(invoice_codes.begin(), invoice_codes.end());
    invoice_codes.erase(std::unique(invoice_codes.begin(), invoice_codes.end()), invoice_codes.end());

    getline(original_invoice_item_csv, line);
    if (line.compare("\"INVOICE_CODE\",\"ITEM_CODE\",\"AMOUNT\",\"QUANTITY\"") != 0) {
        return false;
    }
    extracted_invoice_item_csv.write(line.c_str(), line.length());
    extracted_invoice_item_csv.put('\n');
    while (getline(original_invoice_item_csv, line)) {
        std::stringstream record(line);
        if (getline(record, code, ',')) {
            if (binary_search(invoice_codes.begin(), invoice_codes.end(), code.substr(1, code.length() - 2))) {
                extracted_invoice_item_csv.write(line.c_str(), line.length());
                extracted_invoice_item_csv.put('\n');
            }
        }
    }
    extracted_invoice_item_csv.close();

    return true;
}

/*!
 * \brief reads original invoice file and creates new file containing only records corresponding to the provided customer codes.
 * this function is also responsible for collecting a vector of invoice codes to pass to extract_invoice_items()
 * \param customer_codes vector filled with customer codes to carry over to new file
 * \return true if successful
 */
bool extract_invoices_and_items(const std::vector<std::string> &customer_codes) {
    std::ifstream original_invoice_csv("original_files/invoice.csv");
    std::ofstream extracted_invoice_csv("extracted_files/extracted_invoice.csv");
    std::string line;
    std::string element;
    std::vector<std::string> invoice_codes;

    getline(original_invoice_csv, line);
    if (line.compare("\"CUSTOMER_CODE\",\"INVOICE_CODE\",\"AMOUNT\",\"DATE\"") != 0) {
        return false;
    }
    extracted_invoice_csv.write(line.c_str(), line.length());
    extracted_invoice_csv.put('\n');
    while (getline(original_invoice_csv, line)) {
        std::stringstream record(line);
        if (getline(record, element, ',')) {
            if (binary_search(customer_codes.begin(), customer_codes.end(), element.substr(1, element.length() - 2))) {
                extracted_invoice_csv.write(line.c_str(), line.length());
                extracted_invoice_csv.put('\n');
                // add the invoice code to the vector so that corresponding invoice item records can be identified
                getline(record, element, ',');
                invoice_codes.push_back(element.substr(1, element.length() - 2));
            }
        }
    }
    extracted_invoice_csv.close();

    // extract invoice items to new file
    return extract_invoice_items(invoice_codes);
}


/*!
 * \brief creates new files containing only data related to the specified customers
 * if successful, extracted data files will be written to extracted_files/
 * \param customer_sample_path path to a .csv containing customer codes
 * \return true if the data extraction was successful
 */
bool extract_customer_data(const std::filesystem::path customer_sample_path)
{
    std::vector<std::string> customer_codes;
    if (set_customer_codes(customer_sample_path, customer_codes)) {
        if (extract_customers(customer_codes) && extract_invoices_and_items(customer_codes)){
            return true;
        }
    }

    return false;
}






// GTESTS:

std::string ReadFile(const std::string& filename) {
    std::ifstream file(filename);
    if (!file) {
        return "";
    }
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

    return content;
}

std::string DEFAULT_CUSTOMER_SAMPLE_FILE =
    "\"CUSTOMER_CODE\"\n"
    "\"CUST0000010231\"\n"
    "\"CUST0000010235\"\n";

std::string DEFAULT_CUSTOMER_FILE =
    "\"CUSTOMER_CODE\",\"FIRSTNAME\",\"LASTNAME\"\n"
    "\"CUST0000010231\",\"Maria\",\"Alba\"\n"
    "\"CUST0000010235\",\"George\",\"Lucas\"\n";

std::string DEFAULT_INVOICE_FILE =
    "\"CUSTOMER_CODE\",\"INVOICE_CODE\",\"AMOUNT\",\"DATE\"\n"
    "\"CUST0000010231\",\"IN0000001\",\"105.50\",\"01-Jan-2016\"\n"
    "\"CUST0000010235\",\"IN0000002\",\"186.53\",\"01-Jan-2016\"\n"
    "\"CUST0000010231\",\"IN0000003\",\"114.14\",\"01-Feb-2016\"\n";

std::string DEFAULT_INVOICE_ITEM_FILE =
    "\"INVOICE_CODE\",\"ITEM_CODE\",\"AMOUNT\",\"QUANTITY\"\n"
    "\"IN0000001\",\"MEIJI\",\"75.60\",\"100\"\n"
    "\"IN0000001\",\"POCKY\",\"10.40\",\"250\"\n"
    "\"IN0000001\",\"PUCCHO\",\"19.50\",\"40\"\n"
    "\"IN0000002\",\"MEIJI\",\"113.40\",\"150\"\n"
    "\"IN0000002\",\"PUCCHO\",\"73.13\",\"150\"\n"
    "\"IN0000003\",\"POCKY\",\"16.64\",\"400\"\n"
    "\"IN0000003\",\"PUCCHO\",\"97.50\",\"200\"\n";

std::string DEFAULT_EXPECTED_CUSTOMER =
    "\"CUSTOMER_CODE\",\"FIRSTNAME\",\"LASTNAME\"\n"
    "\"CUST0000010231\",\"Maria\",\"Alba\"\n"
    "\"CUST0000010235\",\"George\",\"Lucas\"\n";

std::string DEFAULT_EXPECTED_INVOICE =
    "\"CUSTOMER_CODE\",\"INVOICE_CODE\",\"AMOUNT\",\"DATE\"\n"
    "\"CUST0000010231\",\"IN0000001\",\"105.50\",\"01-Jan-2016\"\n"
    "\"CUST0000010235\",\"IN0000002\",\"186.53\",\"01-Jan-2016\"\n"
    "\"CUST0000010231\",\"IN0000003\",\"114.14\",\"01-Feb-2016\"\n";

std::string DEFAULT_EXPECTED_INVOICE_ITEM =
    "\"INVOICE_CODE\",\"ITEM_CODE\",\"AMOUNT\",\"QUANTITY\"\n"
    "\"IN0000001\",\"MEIJI\",\"75.60\",\"100\"\n"
    "\"IN0000001\",\"POCKY\",\"10.40\",\"250\"\n"
    "\"IN0000001\",\"PUCCHO\",\"19.50\",\"40\"\n"
    "\"IN0000002\",\"MEIJI\",\"113.40\",\"150\"\n"
    "\"IN0000002\",\"PUCCHO\",\"73.13\",\"150\"\n"
    "\"IN0000003\",\"POCKY\",\"16.64\",\"400\"\n"
    "\"IN0000003\",\"PUCCHO\",\"97.50\",\"200\"\n";

class DreamCandies : public ::testing::Test {
protected:
    void SetUp() override {
        // setup default files
        std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
        std::ofstream original_customer_csv("original_files/customer.csv");
        std::ofstream original_invoice_csv("original_files/invoice.csv");
        std::ofstream original_invoice_item_csv("original_files/invoice_item.csv");

        customer_sample_csv << DEFAULT_CUSTOMER_SAMPLE_FILE;
        original_customer_csv << DEFAULT_CUSTOMER_FILE;
        original_invoice_csv << DEFAULT_INVOICE_FILE;
        original_invoice_item_csv << DEFAULT_INVOICE_ITEM_FILE;
    }

    void TearDown() override {}
};

TEST_F(DreamCandies, happy_path) {
    // no changes to default files

    ASSERT_TRUE(extract_customer_data("customer_samples/customer_sample.csv"));

    std::string actual_customer = ReadFile("extracted_files/extracted_customer.csv");
    EXPECT_EQ(DEFAULT_EXPECTED_CUSTOMER, actual_customer);

    std::string actual_invoice = ReadFile("extracted_files/extracted_invoice.csv");
    EXPECT_EQ(DEFAULT_EXPECTED_INVOICE, actual_invoice);

    std::string actual_invoice_item = ReadFile("extracted_files/extracted_invoice_item.csv");
    EXPECT_EQ(DEFAULT_EXPECTED_INVOICE_ITEM, actual_invoice_item);
}

TEST_F(DreamCandies, incorrect_header_format) {
    std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
    std::string bad_header =
        "\"BAD_HEADER\"\n"
        "\"DATA1\"\n"
        "\"DATA2\"\n";
    customer_sample_csv << bad_header;

    ASSERT_FALSE(extract_customer_data("customer_samples/customer_sample.csv"));
}

TEST_F(DreamCandies, input_file_does_not_exist) {
    std::remove("customer_samples/customer_sample.csv");

    ASSERT_FALSE(extract_customer_data("customer_samples/customer_sample.csv"));
}

TEST_F(DreamCandies, customer_file_does_not_exist) {
    std::remove("original_files/customer.csv");

    ASSERT_FALSE(extract_customer_data("customer_samples/customer_sample.csv"));
}

TEST_F(DreamCandies, invoice_file_does_not_exist) {
    std::remove("original_files/invoice.csv");

    ASSERT_FALSE(extract_customer_data("customer_samples/customer_sample.csv"));
}

TEST_F(DreamCandies, invoice_item_file_does_not_exist) {
    std::remove("original_files/invoice_item.csv");

    ASSERT_FALSE(extract_customer_data("customer_samples/customer_sample.csv"));
}

TEST_F(DreamCandies, empty_path) {
    ASSERT_FALSE(extract_customer_data(""));
}

TEST_F(DreamCandies, empty_input_file) {
    std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
    customer_sample_csv.close();

    ASSERT_FALSE(extract_customer_data("customer_samples/customer_sample.csv"));
}

TEST_F(DreamCandies, subset_of_customers) {
    std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
    std::string customer_sample_file =
    "\"CUSTOMER_CODE\"\n"
    "\"CUST0000010235\"\n";
    customer_sample_csv << customer_sample_file;
    customer_sample_csv.close();

    ASSERT_TRUE(extract_customer_data("customer_samples/customer_sample.csv"));

    std::string expected_customer =
        "\"CUSTOMER_CODE\",\"FIRSTNAME\",\"LASTNAME\"\n"
        "\"CUST0000010235\",\"George\",\"Lucas\"\n";
    std::string actual_customer = ReadFile("extracted_files/extracted_customer.csv");
    EXPECT_EQ(expected_customer, actual_customer);

    std::string expected_invoice =
        "\"CUSTOMER_CODE\",\"INVOICE_CODE\",\"AMOUNT\",\"DATE\"\n"
        "\"CUST0000010235\",\"IN0000002\",\"186.53\",\"01-Jan-2016\"\n";
    std::string actual_invoice = ReadFile("extracted_files/extracted_invoice.csv");
    EXPECT_EQ(expected_invoice, actual_invoice);

    std::string expected_invoice_item =
        "\"INVOICE_CODE\",\"ITEM_CODE\",\"AMOUNT\",\"QUANTITY\"\n"
        "\"IN0000002\",\"MEIJI\",\"113.40\",\"150\"\n"
        "\"IN0000002\",\"PUCCHO\",\"73.13\",\"150\"\n";
    std::string actual_invoice_item = ReadFile("extracted_files/extracted_invoice_item.csv");
    EXPECT_EQ(expected_invoice_item, actual_invoice_item);
}

TEST_F(DreamCandies, subset_of_customers2) {
    std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
    std::string customer_sample_file =
    "\"CUSTOMER_CODE\"\n"
    "\"CUST0000010231\"\n";
    customer_sample_csv << customer_sample_file;
    customer_sample_csv.close();

    ASSERT_TRUE(extract_customer_data("customer_samples/customer_sample.csv"));

    std::string expected_customer =
        "\"CUSTOMER_CODE\",\"FIRSTNAME\",\"LASTNAME\"\n"
        "\"CUST0000010231\",\"Maria\",\"Alba\"\n";
    std::string actual_customer = ReadFile("extracted_files/extracted_customer.csv");
    EXPECT_EQ(expected_customer, actual_customer);

    std::string expected_invoice =
        "\"CUSTOMER_CODE\",\"INVOICE_CODE\",\"AMOUNT\",\"DATE\"\n"
        "\"CUST0000010231\",\"IN0000001\",\"105.50\",\"01-Jan-2016\"\n"
        "\"CUST0000010231\",\"IN0000003\",\"114.14\",\"01-Feb-2016\"\n";
    std::string actual_invoice = ReadFile("extracted_files/extracted_invoice.csv");
    EXPECT_EQ(expected_invoice, actual_invoice);

    std::string expected_invoice_item =
        "\"INVOICE_CODE\",\"ITEM_CODE\",\"AMOUNT\",\"QUANTITY\"\n"
        "\"IN0000001\",\"MEIJI\",\"75.60\",\"100\"\n"
        "\"IN0000001\",\"POCKY\",\"10.40\",\"250\"\n"
        "\"IN0000001\",\"PUCCHO\",\"19.50\",\"40\"\n"
        "\"IN0000003\",\"POCKY\",\"16.64\",\"400\"\n"
        "\"IN0000003\",\"PUCCHO\",\"97.50\",\"200\"\n";
    std::string actual_invoice_item = ReadFile("extracted_files/extracted_invoice_item.csv");
    EXPECT_EQ(expected_invoice_item, actual_invoice_item);
}

TEST_F(DreamCandies, duplicate_customer_codes) {
    std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
    std::string customer_sample_file =
    "\"CUSTOMER_CODE\"\n"
    "\"CUST0000010235\"\n"
    "\"CUST0000010231\"\n"
    "\"CUST0000010235\"\n";
    customer_sample_csv << customer_sample_file;
    customer_sample_csv.close();
    
    ASSERT_TRUE(extract_customer_data("customer_samples/customer_sample.csv"));

    std::string actual_customer = ReadFile("extracted_files/extracted_customer.csv");
    EXPECT_EQ(DEFAULT_EXPECTED_CUSTOMER, actual_customer);

    std::string actual_invoice = ReadFile("extracted_files/extracted_invoice.csv");
    EXPECT_EQ(DEFAULT_EXPECTED_INVOICE, actual_invoice);

    std::string actual_invoice_item = ReadFile("extracted_files/extracted_invoice_item.csv");
    EXPECT_EQ(DEFAULT_EXPECTED_INVOICE_ITEM, actual_invoice_item);
} 

TEST_F(DreamCandies, no_matching_customers) {
    std::ofstream customer_sample_csv("customer_samples/customer_sample.csv");
    std::string customer_sample_file =
    "\"CUSTOMER_CODE\"\n"
    "\"CUST0000010000\"\n";
    customer_sample_csv << customer_sample_file;
    customer_sample_csv.close();

    ASSERT_TRUE(extract_customer_data("customer_samples/customer_sample.csv"));

    std::string expected_customer =
        "\"CUSTOMER_CODE\",\"FIRSTNAME\",\"LASTNAME\"\n";
    std::string actual_customer = ReadFile("extracted_files/extracted_customer.csv");
    EXPECT_EQ(expected_customer, actual_customer);

    std::string expected_invoice =
        "\"CUSTOMER_CODE\",\"INVOICE_CODE\",\"AMOUNT\",\"DATE\"\n";
    std::string actual_invoice = ReadFile("extracted_files/extracted_invoice.csv");
    EXPECT_EQ(expected_invoice, actual_invoice);

    std::string expected_invoice_item =
        "\"INVOICE_CODE\",\"ITEM_CODE\",\"AMOUNT\",\"QUANTITY\"\n";
    std::string actual_invoice_item = ReadFile("extracted_files/extracted_invoice_item.csv");
    EXPECT_EQ(expected_invoice_item, actual_invoice_item);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
