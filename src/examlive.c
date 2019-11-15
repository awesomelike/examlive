#include<mysql/mysql.h>
#include<gtk/gtk.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

//GTK components




//Database credentials
static char *host = "db4free.net";
static char *user = "examlive";
static char *dbname = "examlive";
static char *pass = "examlive";
unsigned int port = 3306;
static char* unix_socket = NULL;
unsigned int flag = 0;

//This function establishes DB connection
void db_connect (MYSQL *conn) {
	conn = mysql_init(NULL);
	if(!(mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag))) {
		printf("%s\n", "some error there gentleman");
		return;
	}
	printf("%s\n", "DB connection successful!");
}

int main(int argc, char *argv[]) {

	//We refer to DB instance with this variable
	MYSQL *conn;
	conn = mysql_init(NULL);
	if(!(mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag))) {
		printf("%s\n", "some error there gentleman");
		return;
	}
	//printf("%s\n", "DB connection successful!");
	
	
	
	if (mysql_query(conn, "SELECT * FROM courses")) {
	 	fprintf(stderr, "Error=%s\n", mysql_error(conn));
	}
	MYSQL_RES *res;
	res = mysql_store_result(conn);
	int num_fields = mysql_num_fields(res);
	MYSQL_ROW *row;
	//printf("%d\n", num_fields);
	while ((row = mysql_fetch_row(res)))
	{
		for(int i=0; i<num_fields; i++) {
			printf("%s ", row[i]);
		}
	}
	
	
	return 0;
}