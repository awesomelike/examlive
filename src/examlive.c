#include<mysql/mysql.h>
#include<signal.h>
#include <sys/types.h>
#include<unistd.h>
#include<gtk/gtk.h>
#include<gtk/gtkx.h>
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include <ctype.h>

//CSS values
GtkCssProvider  *provider;
GdkScreen       *screen;
GdkDisplay      *display;

//Database credentials
static char *host = "db4free.net";
static char *user = "examlive";
static char *dbname = "examlive";
static char *pass = "examlive";
unsigned int port = 3306;
static char* unix_socket = NULL;
unsigned int flag = 0;

MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

//GTK Login Page
GtkWidget	*login_window;
GtkWidget	*login_fixed;
GtkWidget	*login_image_1;
GtkWidget	*login_button;
GtkWidget	*login_username;
GtkWidget	*login_password;
GtkBuilder	*builder; 
GtkWidget   *login_label_error;

//GTK professor main panel Page
GtkWidget *pr_window_panel;
GtkWidget *start_btn;
GtkWidget *pr_swap_panel;
GtkWidget *label_prof_name;
GtkWidget *label_prof_id;

GtkWidget *st_window_panel;
GtkWidget *st_name;
GtkWidget *st_id;
GtkWidget *label_button_student;

//Login panel variables
char user_id[128];
char user_password[128];

struct User
{
	char id[7];
	char password[128];
	char full_name[128];
};

static struct User user_obj;

int main(int argc, char *argv[]) {

	//We refer to DB instance with this variable
	conn = mysql_init(NULL);
	if(!(mysql_real_connect(conn, host, user, pass, dbname, port, unix_socket, flag))) {
		printf("%s\n", "some error there");
		exit(1);
	} else {
		printf("%s\n", "DB conn established");
	}
	

	gtk_init(&argc, &argv); 
 
	builder = gtk_builder_new_from_file ("ui/test.glade");
 
	//main windows variables

	login_window = GTK_WIDGET(gtk_builder_get_object(builder, "login_window"));
	
	g_signal_connect(login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);

    gtk_builder_connect_signals(builder, NULL);

    //declare variables login
	login_fixed = GTK_WIDGET(gtk_builder_get_object(builder, "login_fixed"));
	login_button = GTK_WIDGET(gtk_builder_get_object(builder, "login_button"));
	login_image_1 = GTK_WIDGET(gtk_builder_get_object(builder, "login_image_1"));
	login_username = GTK_WIDGET(gtk_builder_get_object(builder, "login_username"));
	login_password = GTK_WIDGET(gtk_builder_get_object(builder, "login_password"));
	login_label_error = GTK_WIDGET(gtk_builder_get_object(builder, "login_label_error"));

	//declare variable professor panel
	start_btn = GTK_WIDGET(gtk_builder_get_object(builder, "start_btn"));
	pr_swap_panel = GTK_WIDGET(gtk_builder_get_object(builder, "swaping_panel"));
	pr_window_panel=  GTK_WIDGET(gtk_builder_get_object(builder, "pr_window_panel"));
	label_prof_name = GTK_WIDGET(gtk_builder_get_object(builder, "label_prof_name"));
	label_prof_id = GTK_WIDGET(gtk_builder_get_object(builder, "label_prof_id"));

	//declare variable student panel
	st_window_panel = GTK_WIDGET(gtk_builder_get_object(builder, "st_window_panel"));
	st_id = GTK_WIDGET(gtk_builder_get_object(builder, "st_id"));
	st_name = GTK_WIDGET(gtk_builder_get_object(builder, "st_name"));
	label_button_student = GTK_WIDGET(gtk_builder_get_object(builder, "leave_button_student"));

	//These lines used to connect CSS
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),"style/style.css",NULL);

   

    gtk_widget_show(login_window);
	//gtk_widget_show(st_window_panel);

	gtk_main();
		
	return 0;
}
int row_count;
gint x, y;
void on_sign_in_clicked  (GtkButton *b) {

	//printf("%s\n", user_id);
	//printf("%s\n", user_password);
	char sql_select[1024];
	
	if(user_id[0]=='P') 
	{
		sprintf(sql_select, "SELECT id, full_name FROM professors WHERE id='%s' AND password='%s'", user_id, user_password);
		//printf("%s\n", sql_select);
		if(mysql_query(conn, sql_select)) {
    		fprintf(stderr, "%s\n", mysql_error(conn));
			printf("%s\n", "User does not exist!");
			return;
  		}
  		res = mysql_store_result(conn);
		
		if(mysql_num_rows(res)==0) {
			printf("Wrong User ID, or password\n");
			gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "Invalid user id, or password");
			mysql_free_result(res);
		} else { //Successful authentication
			row = mysql_fetch_row(res);
			strcpy(user_obj.id, row[0]);
			strcpy(user_obj.full_name, row[1]);
			gtk_label_set_text(GTK_LABEL(label_prof_id), (const gchar*) user_obj.id);
			gtk_label_set_text(GTK_LABEL(label_prof_name), (const gchar*) user_obj.full_name);
			gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "");
			mysql_free_result(res);
			gtk_widget_hide(login_window);
			gtk_widget_show(pr_window_panel);	
		}
		  
		
		
	} else if (user_id[0]=='S')
	{
		sprintf(sql_select, "SELECT id, full_name FROM students WHERE id='%s' AND password='%s'", user_id, user_password);
		printf("%s\n", sql_select);
		if(mysql_query(conn, sql_select)) {
    		fprintf(stderr, "%s\n", mysql_error(conn));
			printf("%s\n", "User does not exist!");
			return;
  		}
  		res = mysql_store_result(conn);
		
		if(mysql_num_rows(res)==0) {
			printf("Wrong User ID, or password\n");
			gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "Invalid user id, or password");
			mysql_free_result(res);
		} else { 				
			//Successful authentication
			row = mysql_fetch_row(res);
			strcpy(user_obj.id, row[0]);
			strcpy(user_obj.full_name, row[1]);
			gtk_label_set_text(GTK_LABEL(st_id), (const gchar*) user_obj.id);
			gtk_label_set_text(GTK_LABEL(st_name), (const gchar*) user_obj.full_name);
			gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "");
			mysql_free_result(res);
			gtk_widget_hide(login_window);
			gtk_widget_show(st_window_panel);	
		}
	}
	
	


//pr windows current position state
	// gtk_window_get_position(login_window, &x, &y);
	// gtk_window_move (pr_window_panel,x,y);
	// gtk_window_move (login_window,x,y);
	// 	printf("current position SIGN IN PAGE is:\nx: %i\ny:%i\n", x, y);
 //window switches
}
void on_pr_start_quiz_clicked (GtkButton *c){
	
}
void on_login_username_changed(GtkEntry *e){
	sprintf(user_id, "%s", gtk_entry_get_text(e));
}

void on_login_password_changed (GtkEntry *p ){
	sprintf(user_password, "%s", gtk_entry_get_text(p));
}

void on_leave_button_clicked(GtkButton *b){

//pr windows current position state
	// gtk_window_get_position(pr_window_panel, &x, &y);
	// gtk_window_move (login_window, x , y);
	
//window switches
	sprintf(user_obj.full_name, "");
	sprintf(user_obj.full_name, "");
	gtk_entry_set_text(GTK_ENTRY(login_username), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(login_password), (const gchar*) "");
	gtk_widget_hide(pr_window_panel) ;
	gtk_widget_show(login_window);

}
void on_leave_button_student_clicked(GtkButton *b) {
	sprintf(user_obj.full_name, "");
	sprintf(user_obj.full_name, "");
	gtk_entry_set_text(GTK_ENTRY(login_username), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(login_password), (const gchar*) "");
	gtk_widget_hide(st_window_panel) ;
	gtk_widget_show(login_window);
}