#include<mysql/mysql.h>
#include<signal.h>
#include<sys/types.h>
#include<unistd.h>
#include<gtk/gtk.h>
#include<gtk/gtkx.h>
#include<stdio.h>
#include<fcntl.h> 
#include<stdlib.h>
#include<netdb.h> 
#include<string.h>
#include<ctype.h>
#include<sys/socket.h> 
#include<pthread.h>
#include<netinet/in.h> 
#include<arpa/inet.h>
#include "components.h"


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


char sql_select[1024];
char sql_update[1024];

//Utility function prototypes
void clear_question_form();
void get_professor_exams();
void get_professor_courses();
void get_ip_address(char *);

//Login panel variables
char user_id[128];
char user_password[128];

struct User
{
	char id[7];
	char password[128];
	char full_name[128];
};

struct Exam 
{
	int id;
	int course_id;
	char prof_id[7];
	char title[128];
};

static struct User user_obj;
static struct Exam exam_obj;

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
 
	initialize_components();
   
	gtk_widget_set_sensitive(btn_start_exam, FALSE);
	gtk_widget_hide(entry_question);
	gtk_widget_hide(grid_create_answers);
	gtk_widget_set_sensitive(btn_add_question, FALSE);
	gtk_widget_set_sensitive(btn_save_exam, FALSE);
    gtk_widget_show(login_window);

	gtk_main();
		
	return 0;
}
int row_count;
gint x, y;
void on_sign_in_clicked  (GtkButton *b) {
	gtk_widget_show(login_spinner);
	gtk_spinner_start(GTK_SPINNER(login_spinner));
	char sql_select[1024];
	
	if(user_id[0]=='P') 
	{
		sprintf(sql_select, "SELECT id, full_name FROM professors WHERE id='%s' AND password='%s'", user_id, user_password);
		if(mysql_query(conn, sql_select)) {
    		fprintf(stderr, "%s\n", mysql_error(conn));
			printf("%s\n", "User does not exist!");
			return;
  		}
  		res = mysql_store_result(conn);
		
		if(mysql_num_rows(res)==0) {
			printf("Wrong User ID, or password\n");
			gtk_spinner_stop(GTK_SPINNER(login_spinner));
			gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "Invalid user id, or password");
			mysql_free_result(res);
		} else { //Successful authentication
			row = mysql_fetch_row(res);
			strcpy(user_obj.id, row[0]);
			strcpy(user_obj.full_name, row[1]);
			mysql_free_result(res);
			
			get_professor_courses();
			get_professor_exams();
			
			gtk_label_set_text(GTK_LABEL(label_prof_id), (const gchar*) user_obj.id);
			gtk_label_set_text(GTK_LABEL(label_prof_name), (const gchar*) user_obj.full_name);
			gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "");
			gtk_widget_hide(login_window);
			gtk_spinner_stop(GTK_SPINNER(login_spinner));
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
	} else {
		printf("else , Wrong User ID, or password\n");
		gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "Invalid user id, or password");
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

void on_combo_course_changed (GtkComboBox *c) {
	exam_obj.course_id = atoi(gtk_combo_box_get_active_id(combo_course));
}
void on_entry_exam_title_activate (GtkEntry *e) {
	sprintf(exam_obj.title, gtk_entry_get_text(e));
	gtk_combo_box_set_button_sensitivity(GTK_COMBO_BOX(combo_course), GTK_SENSITIVITY_OFF);
	char sql_insert[512];
	sprintf(sql_insert, "INSERT INTO exams (course_id, prof_id, title) VALUES(%d, '%s', '%s')", exam_obj.course_id, user_obj.id, exam_obj.title);
	
	//Modify ui state
	gtk_widget_set_sensitive(e, FALSE);
	gtk_widget_set_sensitive(combo_course, FALSE);
	gtk_widget_set_sensitive(btn_add_question, TRUE);
	gtk_widget_set_sensitive(btn_save_exam, TRUE);
	gtk_widget_show(entry_question);
	gtk_widget_show(grid_create_answers);
	if(mysql_query(conn, sql_insert)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
  	}
	exam_obj.id = mysql_insert_id(conn);
}
void on_combo_start_quiz_changed (GtkComboBox *c) {
	exam_obj.id = atoi(gtk_combo_box_get_active_id(c));
	gtk_widget_set_sensitive(btn_start_exam, TRUE);
}

void on_btn_start_exam_clicked (GtkButton *b) {
	char ip[20];
	get_ip_address(ip);
	sprintf(sql_update, "UPDATE exams SET status=1, ip_address='%s', port_number=7777 WHERE id=%d", ip, exam_obj.id);
	if (mysql_query(conn, sql_update))
	{            
		mysql_errno(conn);         
	}           
	res = mysql_use_result(conn);
	mysql_free_result(res);
    
}

void *showSpinner(void *args) {
	gtk_spinner_start(GTK_SPINNER(spinner_add_question));
}

int num_questions = 0;
void on_btn_add_question_clicked (GtkButton *b) {
	pthread_t id;
	pthread_create(&id, NULL, showSpinner, NULL);
	
	char sql_insert[1024];
	num_questions = num_questions + 1;
	sprintf(sql_insert, "INSERT INTO questions(exam_id, question, question_number) VALUES(%d, '%s', %d)", exam_obj.id, gtk_entry_get_text(entry_question), num_questions);
	if(mysql_query(conn, sql_insert)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
  	}
	int question_id = mysql_insert_id(conn);
	
	sprintf(sql_insert, "INSERT INTO choices(question_id, letter, choice_value, is_correct) VALUES(%d, 'A', '%s', %d)", 
			question_id,  
			gtk_entry_get_text(entry_answer_a), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_a)) ? 1 : 0);
	if(mysql_query(conn, sql_insert)){fprintf(stderr, "%s\n", mysql_error(conn));}
	sprintf(sql_insert, "INSERT INTO choices(question_id, letter, choice_value, is_correct) VALUES(%d, 'B', '%s', %d)", 
			question_id,  
			gtk_entry_get_text(entry_answer_b), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_b)) ? 1 : 0);
	if(mysql_query(conn, sql_insert)){fprintf(stderr, "%s\n", mysql_error(conn));}
	sprintf(sql_insert, "INSERT INTO choices(question_id, letter, choice_value, is_correct) VALUES(%d, 'C', '%s', %d)", 
			question_id,  
			gtk_entry_get_text(entry_answer_c), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_c)) ? 1 : 0);
	if(mysql_query(conn, sql_insert)){fprintf(stderr, "%s\n", mysql_error(conn));}
	sprintf(sql_insert, "INSERT INTO choices(question_id, letter, choice_value, is_correct) VALUES(%d, 'D', '%s', %d)", 
			question_id,  
			gtk_entry_get_text(entry_answer_d), 
			gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radio_d)) ? 1 : 0);
	if(mysql_query(conn, sql_insert)){fprintf(stderr, "%s\n", mysql_error(conn));}
	clear_question_form();
	char text[16];
	snprintf (text, sizeof(text), "%d",num_questions);
	gtk_label_set_text(GTK_LABEL(label_num_questions), (const gchar*) text); 
	
	pthread_join(id, NULL);
	gtk_spinner_stop(GTK_SPINNER(spinner_add_question));
}

void on_btn_save_exam_clicked (GtkButton *b) {
	gtk_list_store_clear(GTK_LIST_STORE(liststore3));
	get_professor_exams();
	gtk_widget_set_sensitive(entry_exam_title, TRUE);
	gtk_widget_set_sensitive(combo_course, TRUE);
	gtk_widget_set_sensitive(btn_add_question, FALSE);
	gtk_widget_set_sensitive(btn_save_exam, FALSE);
	gtk_widget_hide(entry_question);
	gtk_widget_hide(grid_create_answers);
	gtk_entry_set_text(GTK_ENTRY(entry_exam_title), (const gchar*) "");
	num_questions = 0;
	gtk_label_set_text(GTK_LABEL(label_num_questions), (const gchar*) "0"); 
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
	gtk_widget_hide(pr_window_panel);
	gtk_list_store_clear(GTK_LIST_STORE(liststore2));
	gtk_list_store_clear(GTK_LIST_STORE(liststore3));
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


void clear_question_form() {
	gtk_entry_set_text(GTK_ENTRY(entry_question), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_a), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_b), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_c), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_d), (const gchar*) "");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_a), TRUE);
}

void get_professor_exams() {
	sprintf(sql_select, "SELECT id, title FROM exams WHERE prof_id='%s'", user_obj.id);
	if(mysql_query(conn, sql_select)) {
		fprintf(stderr, "%s\n", mysql_error(conn));		
	}
	res = mysql_store_result(conn);
	while ((row = mysql_fetch_row(res)))
	{
		gtk_list_store_insert_with_values(GTK_LIST_STORE(liststore3), NULL, -1, 0, row[0], 1, row[1], -1);	
	}
	mysql_free_result(res);
}

void get_professor_courses() {
	sprintf(sql_select, "SELECT courses.id, courses.title AS course_id FROM teach JOIN courses ON teach.course_id=courses.id WHERE teach.id='%s'", user_obj.id);
	if(mysql_query(conn, sql_select)) {
		fprintf(stderr, "%s\n", mysql_error(conn));
	}
	res = mysql_store_result(conn);
	while ((row = mysql_fetch_row(res)))
	{
		gtk_list_store_insert_with_values(GTK_LIST_STORE(liststore2), NULL, -1, 0, row[0], 1, row[1], -1);	
	}
	mysql_free_result(res);
}

void get_ip_address(char *buf) {
	system("hostname -I > ip.txt");
	FILE * fp;
    char * ip = NULL;
    size_t len = 0;
    ssize_t read;

    fp = fopen("ip.txt", "r");
    if (fp == NULL)
        exit(EXIT_FAILURE);

    while ((read = getline(&ip, &len, fp)) != -1) {

    }
	printf("%d\n", strlen(ip));
	printf("%s\n", ip);
    fclose(fp);
	strcpy(buf, ip);
	system("rm ip.txt");
	if (ip)
        free(ip);
}