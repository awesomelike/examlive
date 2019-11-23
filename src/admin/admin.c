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
#include <stdbool.h>

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
GtkWidget	*label_course_title;
GtkWidget	*label_course_professor;
GtkWidget	*entry_course_title;
GtkWidget	*combo_course_professor;
GtkWidget	*btn_save_course;
GtkListStore *liststore2;

GtkWidget	*window_assign_course_table;
GtkWidget	*grid_assign;
GtkWidget	*button_action[500];

GtkWidget	*window_course_added;
GtkWidget	*fixed4;
GtkWidget	*btn_main_menu1;

//Vars to store values from add user page
char user_type[1] = "S";
char user_full_name[128];

//Vars to store values from add course page
char course_title[128];
char course_pid[7];

//Function prototypes
void on_assign_row_btn_clicked(GtkButton*, int); 

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
	label_course_title = GTK_WIDGET(gtk_builder_get_object(builder, "label_course_title"));
	label_course_professor = GTK_WIDGET(gtk_builder_get_object(builder, "label_course_professor"));
	entry_course_title = GTK_WIDGET(gtk_builder_get_object(builder, "entry_course_title"));
	combo_course_professor = GTK_WIDGET(gtk_builder_get_object(builder, "combo_course_professor"));
	liststore2 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore2"));
	btn_save_course = GTK_WIDGET(gtk_builder_get_object(builder, "btn_save_course"));
	
	window_course_added = GTK_WIDGET(gtk_builder_get_object(builder, "window_course_added"));
	btn_main_menu1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_main_menu1"));

	window_assign_course_table = GTK_WIDGET(gtk_builder_get_object(builder, "window_assign_course_table"));
	grid_assign = GTK_WIDGET(gtk_builder_get_object(builder, "grid_assign"));

	btn_back1 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back1"));
	btn_back2 = GTK_WIDGET(gtk_builder_get_object(builder, "btn_back2"));
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
//This temporary array and variable is to handle duplicate rows in the list store
char ids[64][7];
int row_count = 0;
void on_add_course_clicked (GtkButton *b) {
	gtk_widget_hide(admin_main);
	
	int num_rows;
	if (mysql_query(conn, "SELECT COUNT(*) FROM professors")) {
		fprintf(stderr, "%s\n", mysql_error(conn)); 
	}
	res = mysql_store_result(conn);
	while (row = mysql_fetch_row(res))
	{
		num_rows = row[0];
	}
	printf("%d\n", num_rows);
	mysql_free_result(res); 
	
	if(mysql_query(conn, "SELECT * FROM professors")) {
		fprintf(stderr, "%s\n", mysql_error(conn)); 
	}
	res = mysql_store_result(conn);
	
	while ((row = mysql_fetch_row(res)))
	{
		int exists = 0;
		for (int j=0; j<=row_count; j++) {
			if(strcmp(ids[j], row[0])==0) {
				exists = 1;
				break;
			} 
		}
		if(exists == 0) {
			strcpy(ids[row_count], row[0]);
			gtk_list_store_insert_with_values(liststore2, NULL, -1, 0, row[0], 1, row[2]);		
			row_count = row_count+1;
		}
		
	}
	mysql_free_result(res);	
	gtk_widget_show(window_course);
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

void on_entry_course_title_changed(GtkEntry *e) {
	sprintf(course_title, "%s", gtk_entry_get_text(e));
}

void on_combo_course_professor_changed(GtkComboBox *c) {
	printf("%s\n", gtk_combo_box_get_active_id(combo_course_professor));
	sprintf(course_pid, "%s", gtk_combo_box_get_active_id(combo_course_professor));
}

void on_btn_save_course_clicked(GtkButton *b) {
	char sql_insert[1024];
	sprintf(sql_insert,
			"INSERT INTO courses(title) VALUES('%s')", 
			course_title
			);
	if (mysql_query(conn, sql_insert)==0) {
		int course_insert_id = mysql_insert_id(conn);
		sprintf(sql_insert,
			"INSERT INTO teach(id, course_id) VALUES('%s', %d)", 
			course_pid,
			course_insert_id
			);
		if (mysql_query(conn, sql_insert)==0) {
			printf("%s\n", "Course inserted successfully");
		}
		printf("%s\n", "Inserted successfully");
	}
	gtk_entry_set_text(GTK_ENTRY(entry_course_title), (const gchar*) "");

	gtk_widget_hide(window_course);
	gtk_widget_show(window_course_added);	
}

void on_assign_course_clicked (GtkButton *b) {
	gtk_widget_hide(admin_main);
	if(mysql_query(conn, "SELECT id, full_name FROM students")) {
    	fprintf(stderr, "%s\n", mysql_error(conn)); 
  	}
  	res = mysql_store_result(conn);
	int top = 1; 
	while ((row = mysql_fetch_row(res)))
	{
		int left = 0;
		gtk_grid_attach(GTK_GRID(grid_assign), gtk_label_new((const gchar*) row[left]), left, top, 1, 1);
		left = left + 1;
		gtk_grid_attach(GTK_GRID(grid_assign), gtk_label_new((const gchar*) row[left]), left, top, 1, 1);
		left = left + 1;
		button_action[top] = gtk_button_new_with_label((const gchar*) "Assign");
		gtk_grid_attach(GTK_GRID(grid_assign), button_action[top], left, top, 1, 1);
		g_signal_connect(button_action[top], "clicked", G_CALLBACK(on_assign_row_btn_clicked), (int)top);
		top = top + 1;
	}
	gtk_widget_show_all(window_assign_course_table);
}

void on_assign_row_btn_clicked(GtkButton *b, int top) {
	//printf("top=%d\n", top);
	char student_id[7];
	char student_name[128];
	sprintf(student_id, "%s", gtk_label_get_text(GTK_LABEL(gtk_grid_get_child_at(grid_assign, 0, top))));
	student_id[6] = '\0';
	sprintf(student_name, "%s", gtk_label_get_text(GTK_LABEL(gtk_grid_get_child_at(grid_assign, 1, top))));
	printf("%s\n", student_id);
	printf("%s\n", student_name);
}

void on_btn_main_menu1_clicked(GtkButton *b) {	
	gtk_widget_hide(window_course_added);
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
