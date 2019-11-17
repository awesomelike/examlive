#include <mysql/mysql.h>
#include <stdlib.h>
#include <sys/types.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <math.h>
#include <time.h>
#include <ctype.h>

//Database credentials
static char *host = "db4free.net";
static char *user = "examlive";
static char *dbname = "examlive";
static char *pass = "examlive";
unsigned int port = 3306;
static char* unix_socket = NULL;
unsigned int flag = 0;

//MySQL variables
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

// Make them global
GtkWidget	*admin_main;
GtkWidget	*fixed1;
GtkWidget   *label_main;
GtkWidget	*add_user;
GtkWidget	*add_course;
GtkBuilder	*builder; 

GtkWidget   *window_user;
GtkWidget   *fixed2;
GtkWidget	*btn_back1;
GtkWidget	*combo1;
GtkWidget	*entry1;
GtkWidget	*entry_name;
GtkWidget	*btn_save_user;

GtkWidget	*window_user_added;
GtkWidget	*type_value;
GtkWidget	*name_value;
GtkWidget	*id_value;
GtkWidget	*password_value;

GtkWidget 	*window_course;
GtkWidget 	*fixed3;
GtkWidget	*label3;
GtkWidget	*btn_back2;



char user_type[1] = "S";
char user_full_name[128];


int main(int argc, char *argv[]) {
	
	conn = mysql_init(NULL);
	if(!(mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag))) {
		printf("%s\n", "some error there");
		exit(1);
	} else {
		printf("%s\n", "DB conn established");
	}

	gtk_init(&argc, &argv); // init Gtk


	builder = gtk_builder_new_from_file ("examlive-admin.glade");
 
	admin_main = GTK_WIDGET(gtk_builder_get_object(builder, "admin_main"));

	g_signal_connect(admin_main, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    gtk_builder_connect_signals(builder, NULL);

	fixed1 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed1"));
    label_main = GTK_WIDGET(gtk_builder_get_object(builder, "label_main"));
    add_user = GTK_WIDGET(gtk_builder_get_object(builder, "add_user"));
    add_course = GTK_WIDGET(gtk_builder_get_object(builder, "add_course"));
    
	window_user = GTK_WIDGET(gtk_builder_get_object(builder, "window_user"));
	fixed2 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed2"));
    combo1 = GTK_WIDGET(gtk_builder_get_object(builder, "combo1"));
	entry1 = GTK_WIDGET(gtk_builder_get_object(builder, "entry1"));
	entry_name = GTK_WIDGET(gtk_builder_get_object(builder, "entry_name"));
	btn_save_user = GTK_WIDGET(gtk_builder_get_object(builder, "btn_save_user"));

	window_user_added = GTK_WIDGET(gtk_builder_get_object(builder, "window_user_added"));
	type_value = GTK_WIDGET(gtk_builder_get_object(builder, "type_value"));
	name_value = GTK_WIDGET(gtk_builder_get_object(builder, "name_value"));
	id_value = GTK_WIDGET(gtk_builder_get_object(builder, "id_value"));
	password_value = GTK_WIDGET(gtk_builder_get_object(builder, "password_value"));

	window_course = GTK_WIDGET(gtk_builder_get_object(builder, "window_course"));
	fixed3 = GTK_WIDGET(gtk_builder_get_object(builder, "fixed3"));
    label3 = GTK_WIDGET(gtk_builder_get_object(builder, "label3"));

	btn_back1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back1"));
	btn_back1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back2"));
	gtk_widget_show(admin_main);

	gtk_main();

	return EXIT_SUCCESS;
	}
//Handlers in main menu
void on_add_user_clicked (GtkButton *b) {
    gtk_widget_hide(admin_main);
	gtk_widget_show(window_user);

	sprintf(user_type, "%s", "S");
	sprintf(user_full_name, "%s", NULL);
}
void on_add_course_clicked (GtkButton *b) {
	gtk_widget_hide(admin_main);
	gtk_widget_show(window_user);
}

//Handlers in add user
void on_combo1_changed (GtkComboBox *c) {
	//Nothing here so far
}

void on_entry1_changed (GtkEntry *e) {
	//If a Professor type is chosen
	if(strcmp(gtk_entry_get_text(e), "Student")) {
		sprintf(user_type, "%s", "P");
	} 
}

void on_entry_name_changed (GtkEntry *e) {
	sprintf(user_full_name, "%s", gtk_entry_get_text(e));
}

void on_btn_save_user_clicked (GtkButton *b) {
	char password[7];
	srand(time(0));
    for(int i=0; i<6; i++) {
        char temp;
        temp = rand()%10 + '0';
        password[i] = temp;
    }
	//We should always terminate a string with a null character
	password[6] = '\0';
	char sql_insert[1024];
	if (user_type[0] == 'S') {
		sprintf(sql_insert,
			"INSERT INTO students(password, full_name) VALUES('%s','%s')", 
			password, 
			user_full_name);
		printf("%s\n", sql_insert);	
		if(mysql_query(conn, sql_insert)==0) {
			gtk_widget_hide(window_user);
			char sql_select[1024];
			sprintf(sql_select, "SELECT id FROM students WHERE full_name='%s'", user_full_name);
			if(mysql_query(conn, sql_select)) {
				fprintf(stderr, "%s\n", mysql_error(conn));
			}
			res = mysql_store_result(conn);
			char id[7];
			while ((row = mysql_fetch_row(res)))
			{
				strcpy(id, row[0]);
			}
			
			gtk_label_set_text(GTK_LABEL(type_value), (const gchar*) "Student");
			gtk_label_set_text(GTK_LABEL(name_value), (const gchar*) user_full_name);
			gtk_label_set_text(GTK_LABEL(id_value), (const gchar*) id);
			gtk_label_set_text(GTK_LABEL(password_value), (const gchar*) password);
			gtk_entry_set_text(GTK_ENTRY(entry_name), (const gchar*) "");
			printf("%s/n", "I am here");
			gtk_widget_hide(window_user);
			gtk_widget_show(window_user_added);
		} else {
			printf("%s\n", "DB error!");
		}
		res = mysql_use_result(conn);
        mysql_free_result(res);
	} else {
		sprintf(sql_insert,
			"INSERT INTO professors(password, full_name) VALUES('%s','%s')", 
			password, 
			user_full_name);
		printf("%s\n", sql_insert);	
		if(mysql_query(conn, sql_insert)==0) {
			gtk_widget_hide(window_user);
			char sql_select[1024];
			sprintf(sql_select, "SELECT id FROM professors WHERE full_name='%s'", user_full_name);
			if(mysql_query(conn, sql_select)) {
				fprintf(stderr, "%s\n", mysql_error(conn));
			}
			res = mysql_store_result(conn);
			char id[7];
			while ((row = mysql_fetch_row(res)))
			{
				strcpy(id, row[0]);
			}
			
			gtk_label_set_text(GTK_LABEL(type_value), (const gchar*) "Professor");
			gtk_label_set_text(GTK_LABEL(name_value), (const gchar*) user_full_name);
			gtk_label_set_text(GTK_LABEL(id_value), (const gchar*) id);
			gtk_label_set_text(GTK_LABEL(password_value), (const gchar*) password);
			gtk_entry_set_text(GTK_ENTRY(entry_name), (const gchar*) "");
			printf("%s/n", "I am here");
			gtk_widget_hide(window_user);
			gtk_widget_show(window_user_added);
		} else {
			printf("%s\n", "DB error!");
		}
		res = mysql_use_result(conn);
        mysql_free_result(res);
	}
}

void on_btn_main_menu_clicked (GtkButton *b) {
	gtk_widget_hide(window_user_added);
	gtk_widget_show(admin_main);
}

void on_btn_back1_clicked (GtkButton *b) {
	gtk_widget_hide(window_user);
	gtk_widget_show(admin_main);
}
void on_btn_back2_clicked (GtkButton *b) {
	gtk_widget_hide(window_course);
	gtk_widget_show(admin_main);
}
