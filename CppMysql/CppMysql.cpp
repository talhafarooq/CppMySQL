// CppMysql.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

/* Standard C++ headers */
#include <iostream>
#include <sstream>
#include <memory>
#include <string>
#include <stdexcept>

/* Mysql Connector/C++ specific headers*/
#include <jdbc/mysql_driver.h>
#include <jdbc/mysql_connection.h>
#include <jdbc/cppconn/statement.h>
#include <jdbc/cppconn/prepared_statement.h>
#include <jdbc/cppconn/resultset.h>
#include <jdbc/cppconn/metadata.h>
#include <jdbc/cppconn/resultset_metadata.h>
#include <jdbc/cppconn/exception.h>
#include <jdbc/cppconn/warning.h>

#define DBHOST "tcp://sql12.freemysqlhosting.net:3306"
#define USER "sql12389628"
#define PASSWORD "xJWUtIi2fJ"

#define DATABASE "sql12389628"

#define NUMOFFSET 100
#define COLNAME 200

using namespace std;
using namespace sql;


static void retrieve_data_and_print(ResultSet* rs, int type, int colidx, string colname) 
{
    /* retrieve the row count in the result set */
    cout << "\nRetrieved " << rs->rowsCount() << " row(s)." << endl;
    cout << "\nCityName" << endl;
    cout << "--------" << endl;
    /* fetch the data : retrieve all the rows in the result set */
    while (rs->next()) {
        if (type == NUMOFFSET) {
            cout << rs->getString(colidx) << endl;
        }
        else if (type == COLNAME) {
            cout << rs->getString(colname) << endl;
        } // if-else
    } // while
    cout << endl;
} // retrieve_data_and_print()

static void retrieve_dbmetadata_and_print(Connection* dbcon) 
{
    if (dbcon->isClosed()) {
        throw runtime_error("DatabaseMetaData FAILURE - database connection closed");
    }
    cout << "\nDatabase Metadata" << endl;
    cout << "-----------------" << endl;
    cout << boolalpha;
    /* The following commented statement won't work with Connector/C++ 1.0.5 and later */
    //auto_ptr < DatabaseMetaData > dbcon_meta (dbcon -> getMetaData());
    DatabaseMetaData* dbcon_meta = dbcon->getMetaData();
    cout << "Database Product Name: " << dbcon_meta->getDatabaseProductName() << endl;
    cout << "Database Product Version: " << dbcon_meta->getDatabaseProductVersion() << endl;
    cout << "Database User Name: " << dbcon_meta->getUserName() << endl << endl;
    cout << "Driver name: " << dbcon_meta->getDriverName() << endl;
    cout << "Driver version: " << dbcon_meta->getDriverVersion() << endl << endl;
    cout << "Database in Read-Only Mode?: " << dbcon_meta->isReadOnly() << endl;
    cout << "Supports Transactions?: " << dbcon_meta->supportsTransactions() << endl;
    cout << "Supports DML Transactions only?: " << dbcon_meta ->
        supportsDataManipulationTransactionsOnly() << endl;
    cout << "Supports Batch Updates?: " << dbcon_meta->supportsBatchUpdates() << endl;
    cout << "Supports Outer Joins?: " << dbcon_meta->supportsOuterJoins() << endl;
    cout << "Supports Multiple Transactions?: " << dbcon_meta ->
        supportsMultipleTransactions() << endl;
    cout << "Supports Named Parameters?: " << dbcon_meta->supportsNamedParameters() << endl;
    cout << "Supports Statement Pooling?: " << dbcon_meta->supportsStatementPooling() << endl;
    cout << "Supports Stored Procedures?: " << dbcon_meta->supportsStoredProcedures() << endl;
    cout << "Supports Union?: " << dbcon_meta->supportsUnion() << endl << endl;
    cout << "Maximum Connections: " << dbcon_meta->getMaxConnections() << endl;
    cout << "Maximum Columns per Table: " << dbcon_meta->getMaxColumnsInTable() << endl;
    cout << "Maximum Columns per Index: " << dbcon_meta->getMaxColumnsInIndex() << endl;
    cout << "Maximum Row Size per Table: " << dbcon_meta->getMaxRowSize() << " bytes" << endl;
    cout << "\nDatabase schemas: " << endl;
    auto_ptr < ResultSet > rs(dbcon_meta->getSchemas());
    cout << "\nTotal number of schemas = " << rs->rowsCount() << endl;
    cout << endl;
    int row = 1;
    while (rs->next()) {
        cout << "\t" << row << ". " << rs->getString("TABLE_SCHEM") << endl;
        ++row;
    } // while
    cout << endl << endl;
} // retrieve_dbmetadata_and_print()


static void retrieve_rsmetadata_and_print(ResultSet* rs) 
{
    if (rs->rowsCount() == 0) {
        throw runtime_error("ResultSetMetaData FAILURE - no records in the result set");
    }
    cout << "ResultSet Metadata" << endl;
    cout << "------------------" << endl;
    /* The following commented statement won't work with Connector/C++ 1.0.5 and later */
    //auto_ptr < ResultSetMetaData > res_meta ( rs -> getMetaData() );
    ResultSetMetaData* res_meta = rs->getMetaData();
    int numcols = res_meta->getColumnCount();
    cout << "\nNumber of columns in the result set = " << numcols << endl << endl;
    cout.width(20);
    cout << "Column Name/Label";
    cout.width(20);
    cout << "Column Type";
    cout.width(20);
    cout << "Column Size" << endl;
    for (int i = 0; i < numcols; ++i) {
        cout.width(20);
        cout << res_meta->getColumnLabel(i + 1);
        cout.width(20);
        cout << res_meta->getColumnTypeName(i + 1);
        cout.width(20);
        cout << res_meta->getColumnDisplaySize(i + 1) << endl << endl;
    }
    cout << "\nColumn \"" << res_meta->getColumnLabel(1);
    cout << "\" belongs to the Table: \"" << res_meta->getTableName(1);
    cout << "\" which belongs to the Schema: \"" << res_meta->getSchemaName(1) << "\"" <<
        endl << endl;
} // retrieve_rsmetadata_and_print()


int main(int argc, const char* argv[])
{
    //std::cout << "Hello World!\n";

    Driver* driver;
    Connection* con;
    Statement* stmt;
    ResultSet* res;
    PreparedStatement* prep_stmt;
    Savepoint* savept;

    int updatecount = 0;

    /* initiate url, user, password and database variable */
    string url(argc >= 2 ? argv[1] : DBHOST);
    const string user(argc >= 3 ? argv[2] : USER);
    const string password(argc >= 4 ? argv[3] : PASSWORD);
    const string database(argc >= 5 ? argv[4] : DATABASE);
    try
    {
        driver = get_driver_instance();
        
        /* create a database connection using the Driver*/
        cout << url << endl;
        con = driver->connect(url, user, password);
        //auto_ptr<Connection> con (driver -> connect(url, user, password));

        /* turn off the autocommit */
        con->setAutoCommit(0);
        cout << "\nDatabase connection\'s autocommit mode = " << con->getAutoCommit() << endl;
        /* select appropriate database schema */
        con->setSchema(database);
        /* retrieve and display the database metadata */
        retrieve_dbmetadata_and_print(con);
        /* create a statement object */
        stmt = con->createStatement();
        cout << "Executing the Query: \"SELECT * FROM City\" .." << endl;
        /* run a query which returns exactly one result set */
        res = stmt->executeQuery("SELECT * FROM CITY");
        cout << "Retrieving the result set .." << endl;

        /* retrieve the data from the result set and display on stdout */
        retrieve_data_and_print(res, NUMOFFSET, 1, string("CityName"));
        /* retrieve and display the result set metadata */
        retrieve_rsmetadata_and_print(res);
        cout << "Demonstrating Prepared Statements .. " << endl << endl;
        /* insert couple of rows of data into City table using Prepared Statements */
        prep_stmt = con->prepareStatement("INSERT INTO CITY (CityName) VALUES (?)");
        cout << "\tInserting \"Moscow, Russia\" into the table, CITY .." << endl;
        prep_stmt->setString(1, "Moscow, Russia");
        updatecount = prep_stmt->executeUpdate();
        cout << "\tCreating a save point \"SAVEPT1\" .." << endl;
        savept = con->setSavepoint("SAVEPT1");
        cout << "\tInserting \"Paris, France\" into the table, CITY .." << endl;
        
        prep_stmt->setString(1, "Paris, France");
        updatecount = prep_stmt->executeUpdate();
        cout << "\tRolling back until the last save point \"SAVEPT1\" .." << endl;
        con->rollback(savept);
        con->releaseSavepoint(savept);
        cout << "\tCommitting outstanding updates to the database .." << endl;
        con->commit();
        cout << "\nQuerying the City table again .." << endl;
        /* re-use result set object */
        res = NULL;
        res = stmt->executeQuery("SELECT * FROM CITY");
        /* retrieve the data from the result set and display on stdout */
        retrieve_data_and_print(res, COLNAME, 1, string("CityName"));


        cout << "Cleaning up the resources .." << endl;
        /* Clean up */
        delete res;
        delete stmt;
        delete prep_stmt;
        con->close();
        delete con;

    }
    catch (SQLException& e)
    {
        cout << "ERROR: SQLException in " << __FILE__;
        cout << " (" << __func__ << ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what();
        cout << " (MySQL error code: " << e.getErrorCode();
        cout << ", SQLState: " << e.getSQLState() << ")" << endl;
        if (e.getErrorCode() == 1047) {
            /*
            Error: 1047 SQLSTATE: 08S01 (ER_UNKNOWN_COM_ERROR)
            Message: Unknown command
            */
            cout << "\nYour server does not seem to support Prepared Statements at all. ";
            cout << "Perhaps MYSQL < 4.1?" << endl;
        }
        return EXIT_FAILURE;
    }
    catch (std::runtime_error& e)
    {
        cout << "ERROR: runtime_error in " << __FILE__;
        cout << " (" << __func__ << ") on line " << __LINE__ << endl;
        cout << "ERROR: " << e.what() << endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
