#include <mysql/mysql.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <gtk/gtk.h>
#include <gtk/gtkx.h>
#include <stdio.h>
#include <fcntl.h> 
#include <stdlib.h>
#include <netdb.h> 
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <sys/socket.h> 
#include <pthread.h>
#include <netinet/in.h> 
#include <arpa/inet.h>
#include "components.h"
#include "server.h"

#define PORT 7777
#define START 1
#define FINISH 0

#define MSG_LEN 1024


//Database credentials
static char *host = "localhost";
static char *user = "root";
static char *dbname = "examlive";
static char *pass = "root";
unsigned int port = 3306;
static char *unix_socket = NULL;
unsigned int flag = 0;

//mysql connection
MYSQL *conn;
MYSQL_RES *res;
MYSQL_ROW row;

//sql request array
char sql_select[1024];
char sql_insert[1024];
char sql_update[1024];

//Custom signal handlers
void join_exam(GtkButton*, int); 

//Utility function prototypes
void clear_question_form();
void get_professor_exams();
void append_new_exam();
void get_history();
void get_student_history();
void get_professor_courses();
void get_ip_address(char *);
void get_next_question();
void split_string(char buf[], char* array[]);
void signal_handler(int);

//Login panel variables
char user_id[128];
char user_password[128];

//Variables to keep track of joined students
StudentList *head, *cur;

/*Global socket variables */
//Server side variables
int s_server_fd, s_client_socket, valread; 
struct sockaddr_in s_serv_address, s_client_address;
//Client side variables
int c_server_socket = 0, valread; 
struct sockaddr_in c_serv_addr; 

int GLOBAL_EXAM_STATUS;

//temp store for update data ///////
char user_current_id[128];
char user_current_password[128];
char user_new_password[128];

int session_id;

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
		printf("%s\n", mysql_error(conn));
		exit(1);
	} else {
		printf("%s\n", "DB conn established");
	}
	
	gtk_init(&argc, &argv); 
	
	signal(SIGINT, signal_handler);

	initialize_components();
   
	gtk_widget_set_sensitive(btn_start_exam, FALSE);
	gtk_widget_hide(entry_question);
	gtk_widget_hide(grid_create_answers);
	gtk_widget_set_sensitive(btn_add_question, FALSE);
	gtk_widget_set_sensitive(btn_save_exam, FALSE);
    gtk_widget_show(login_window);
	gtk_widget_hide(grid_student_results);
	gtk_widget_hide(spinner_results);
	gtk_widget_hide(grid_exam_results);
	gtk_widget_hide(btn_finish);
	gtk_widget_hide(label_exam_finished);
	gtk_widget_hide(scroll_exam);
	gtk_widget_hide(label_results_announced);
	gtk_widget_set_sensitive(btn_finish_exam, FALSE);
	gtk_builder_connect_signals(builder, NULL);
	gtk_main();
		
	return 0;
}
int row_count;

gint x, y;

void signal_handler(int sig) {
	if(GLOBAL_EXAM_STATUS==START && user_obj.id[0]=='P') {
		update_exam_status(FINISH);
	}
	printf("\n");
	exit(EXIT_SUCCESS);
}

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
		} else {     
			//Successful authentication
			row = mysql_fetch_row(res);
			strcpy(user_obj.id, row[0]);
			strcpy(user_obj.full_name, row[1]);
			mysql_free_result(res);
			
			get_professor_courses();
			
			get_professor_exams();
			
			get_history();
			
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
			
			get_student_history();
			
			get_online_exams();
			
			gtk_widget_show_all(st_window_panel);	
		}
	} else {
		printf("Wrong User ID, or password\n");
		gtk_label_set_text(GTK_LABEL(login_label_error), (const gchar*) "Invalid user id, or password");
		gtk_spinner_stop(login_spinner);
	}
	
//pr windows current position state
	// gtk_window_get_position(login_window, &x, &y);
	// gtk_window_move (pr_window_panel,x,y);
	// gtk_window_move (login_window,x,y);
	// 	printf("current position SIGN IN PAGE is:\nx: %i\ny:%i\n", x, y);
 //window switches
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
int total_position;
int exam_student_count = 1;
void request_handler_thread(void *s) {
	char recv_buffer[MSG_LEN];
	StudentList *student = (StudentList*) s;
	if(recv(student->fd, recv_buffer, MSG_LEN, 0) <=0) {
		printf("%s\n","Wrong request");
	} else if (recv_buffer[0]=='$') {
		printf("%s\n","SERVER: Received a join request");
		char sess_str[64];
		sprintf(sess_str, "%d", session_id);
		send(student->fd, sess_str, strlen(sess_str), 0);
		char *temp_arr[4];
		split_string(recv_buffer, temp_arr);
		printf("%s\n", temp_arr[1]);
		strncpy(student->id, temp_arr[1], (size_t)7);
		printf("%s\n", temp_arr[2]);
		printf("%s\n", temp_arr[3]);
		strncpy(student->name, temp_arr[3], (size_t)atoi(temp_arr[2]));
		gtk_widget_hide(grid_student_results);
		gtk_grid_attach(GTK_GRID(grid_student_results), gtk_label_new((const gchar*)student->id), 0, exam_student_count, 1, 1);
		gtk_grid_attach(GTK_GRID(grid_student_results), gtk_label_new((const gchar*)student->name), 1, exam_student_count, 1, 1);
		student->table_position = exam_student_count;
		gtk_grid_attach(GTK_GRID(grid_student_results), gtk_label_new((const gchar*)"0"), total_position, student->table_position, 1, 1);
		gtk_widget_show_all(grid_student_results);
		exam_student_count = exam_student_count + 1;
		memset(recv_buffer, 0, MSG_LEN);
	}
	while (1)
	{
		int receive = recv(student->fd, recv_buffer, MSG_LEN, 0);
		if(receive>0) {
			printf("%s sent following answer: %s\n", student->name, recv_buffer);
			char *temp_ans[6];
			split_string(recv_buffer, temp_ans);
			printf("%s\n", temp_ans[1]);
			printf("%s\n", temp_ans[2]);
			printf("%s\n", temp_ans[3]);
			printf("%s\n", temp_ans[4]);
			sprintf(sql_select, "SELECT exams.id AS exam_id, questions.question_number AS question_number, choices.letter, choices.is_correct FROM exams JOIN questions ON exams.id=questions.exam_id JOIN choices ON questions.id=choices.question_id WHERE exams.id=%d AND questions.question_number=%d AND choices.letter='%s'", atoi(temp_ans[2]), atoi(temp_ans[3]), temp_ans[4]);
			if(mysql_query(conn, sql_select)) {
				fprintf(stderr, "%s\n", mysql_error(conn));
  			}
			res = mysql_store_result(conn);
			while ((row = mysql_fetch_row(res)))
			{
				int is_correct = atoi(row[3]);
				student->score += is_correct;
				int question_number = atoi(temp_ans[3]);
				sprintf(sql_insert, "INSERT INTO responses(student_id, exam_id, answer, score, question_number, session_id) VALUES('%s', %d, '%s', %d, %d, %d)", student->id, atoi(temp_ans[2]), temp_ans[4], is_correct, atoi(temp_ans[3]), session_id);
				if(mysql_query(conn, sql_insert)) {
					fprintf(stderr, "%s\n", mysql_error(conn));
  				}
				gtk_widget_hide(grid_student_results);
				gtk_grid_attach(GTK_GRID(grid_student_results), strcmp(row[3], "0")?gtk_image_new_from_file((const gchar*)"./style/tick.png"):gtk_image_new_from_file((const gchar*)"./style/cross.png") , question_number + 1, student->table_position, 1, 1);
				char score_temp[3];
				sprintf(score_temp, "%d", student->score);
				gtk_label_set_text(GTK_LABEL(gtk_grid_get_child_at(GTK_GRID(grid_student_results), total_position, student->table_position)), (const gchar*)score_temp);
				memset(score_temp, 0, 3);				
				gtk_widget_show_all(grid_student_results);
			}
					
		} else if(receive==0){
 
        } else
		{
			fprintf(stderr, "request handler socket() failed: %s\n", strerror(errno));
		}
		
		
	}
	
}
void table_thread() {
	sprintf(sql_select, "SELECT * FROM questions WHERE exam_id=%d", exam_obj.id);
	if(mysql_query(conn, sql_select)) {
		return fprintf(stderr, "%s\n", mysql_error(conn));
		exit(EXIT_FAILURE);
  	}
	res = mysql_store_result(conn);
	int num_questions = mysql_num_rows(res);
	int i=1;
	while (i<=num_questions)
	{
		gtk_grid_insert_column(GTK_GRID(grid_student_results), (gint)(i + 1));
		i=i+1;
	}
	i=1;
	while (i<=num_questions)
	{
		char str_i[4];
		sprintf(str_i, "Q%d", i);
		gtk_grid_attach(GTK_GRID(grid_student_results), gtk_label_new((const gchar*)str_i), i+1, 0, 1, 1);
		i=i+1;
	}
	total_position = i+2;
	gtk_grid_attach(GTK_GRID(grid_student_results), gtk_label_new((const gchar*)"Total"), i+2, 0, 1, 1);
	gtk_widget_set_sensitive(btn_finish_exam, TRUE);
	gtk_widget_set_sensitive(combo_course, FALSE);
	gtk_widget_show_all(grid_student_results);
}

void server_socket_thread() {
    int opt = 1; 
    int s_server_addrlen = sizeof(s_serv_address);
	int s_client_addrlen = sizeof(s_client_address); 
    char buffer[1024] = {0}; 
       
    // // Creating socket file descriptor 
    if ((s_server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) 
    { 
        perror("socket failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (setsockopt(s_server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, 
                                                  &opt, sizeof(opt))) 
    { 
        perror("setsockopt"); 
        exit(EXIT_FAILURE); 
    } 
    s_serv_address.sin_family = AF_INET; 
    s_serv_address.sin_addr.s_addr = INADDR_ANY; 
    s_serv_address.sin_port = htons( PORT ); 
    
    if (bind(s_server_fd, (struct sockaddr *)&s_serv_address,  
                                 sizeof(s_serv_address))<0) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } 
    if (listen(s_server_fd, 5) < 0) 
    { 
        perror("listen"); 
        exit(EXIT_FAILURE); 
    }
	char serv_ip[32];
	get_ip_address(serv_ip);
	head = newNode(s_server_fd, serv_ip);
	cur = head;
	
	while (1)
	{
		if ((s_client_socket = accept(s_server_fd, (struct sockaddr *)&s_serv_address, (socklen_t*)&s_server_addrlen))<0) { 
			perror("accept"); 
			exit(EXIT_FAILURE); 
		}

		getpeername(s_client_socket, (struct sockaddr*) &s_client_address, (struct socklen_t*)&s_client_address);
		printf("Client ip: %s\n", inet_ntoa(s_client_address.sin_addr)); 
		
		StudentList *s = newNode(s_client_socket, inet_ntoa(s_client_address.sin_addr));
		s->prev = cur;
		cur->next = s;
		cur = s;

		pthread_t threadID;
		if(pthread_create(&threadID, NULL, (void*)request_handler_thread, (void*)s) != 0) {
			perror("Thread creation error!\n");
			exit(EXIT_FAILURE);
		}
	}
	 
}

void on_btn_start_exam_clicked (GtkButton *b) {
    update_exam_status(START);
	
	pthread_t thread_populate;
	if(pthread_create(&thread_populate, NULL, (void*)table_thread, NULL)!=0) {
		perror("Thread error!");
		exit(EXIT_FAILURE);
	}
	
	pthread_t socket_tid;
	if(pthread_create(&socket_tid, NULL, (void*)server_socket_thread, NULL)!=0) {
		perror("Thread error!");
		exit(EXIT_FAILURE);
	} 
}

void *showSpinner(void *args) {
	gtk_spinner_start(GTK_SPINNER(spinner_add_question));
}

int num_questions = 0;
void on_btn_add_question_clicked (GtkButton *b) {
	pthread_t id;
	pthread_create(&id, NULL, showSpinner, NULL);
	if(!gtk_widget_get_sensitive(btn_save_exam)) {
		gtk_widget_set_sensitive(btn_save_exam, TRUE);
	}
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
	append_new_exam();
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
	exit(EXIT_SUCCESS);
//pr windows current position state
	// gtk_window_get_position(pr_window_panel, &x, &y);
	// gtk_window_move (login_window, x , y);
	
//window switches

}
void on_leave_button_student_clicked(GtkButton *b) {
	exit(EXIT_SUCCESS);
}

void clear_question_form() {
	gtk_entry_set_text(GTK_ENTRY(entry_question), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_a), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_b), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_c), (const gchar*) "");
	gtk_entry_set_text(GTK_ENTRY(entry_answer_d), (const gchar*) "");
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(radio_a), TRUE);
}

void get_online_exams() {
	sprintf(sql_select, 
			"SELECT p.full_name, c.title AS course_title, e.title AS exam_title, e.id AS exam_id FROM exams e JOIN professors p ON e.prof_id=p.id JOIN courses c ON e.course_id=c.id WHERE e.course_id IN (SELECT course_id FROM take WHERE id='%s') AND e.status=1", user_obj.id);
	if(mysql_query(conn, sql_select)) {
    	fprintf(stderr, "%s\n", mysql_error(conn)); 
  	}
  	res = mysql_store_result(conn);
	int y=1;
	while ((row = mysql_fetch_row(res)))
	{
		int x=0;
		gtk_grid_attach(GTK_GRID(grid_exams), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_exams), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_exams), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_exams), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_exams), gtk_image_new_from_icon_name((const gchar*) "software-update-available", 1), x, y, 1, 1);
		x = x + 1;
		join_button[y] = gtk_button_new_with_label((const gchar*) "Join");
		gtk_widget_set_name (join_button[y], (const gchar*)("btn-primary"));
		gtk_grid_attach(GTK_GRID(grid_exams), join_button[y], x, y, 1, 1);
		int id = atoi(row[3]);
		g_signal_connect(join_button[y], "clicked", G_CALLBACK(join_exam), y);
		y = y + 1;
	} 
	mysql_free_result(res);
}
int current_question = 1;
void get_next_question() {

	char temp[3];
	sprintf(temp, "%d", current_question);
	gtk_label_set_text(GTK_LABEL(label_exam_question_number), (const gchar*)temp);
	
	sprintf(sql_select, "SELECT id as question_id, question, choices.letter, choices.choice_value FROM questions JOIN choices ON questions.id=choices.question_id WHERE exam_id=%d AND question_number=%d", exam_obj.id, current_question);
	if(mysql_query(conn, sql_select)) {
		fprintf(stderr, "%s\n", mysql_error(conn)); 
	}
	
	res = mysql_store_result(conn);
	
	while ((row = mysql_fetch_row(res)))
	{
		char answer[128];
		gtk_label_set_text(GTK_LABEL(label_exam_question), (const gchar*)row[1]);
		if(strcmp(row[2], "A")==0) {	
			sprintf(answer, "A) ");
			strcat(answer, row[3]);
			gtk_button_set_label(GTK_BUTTON(exam_answer_a), (const gchar*)answer);
			memset(answer, 0, 128);
		}
		if(strcmp(row[2], "B")==0) {
			sprintf(answer, "B) ");
			strcat(answer, row[3]);
			gtk_button_set_label(GTK_BUTTON(exam_answer_b), (const gchar*)answer);
			memset(answer, 0, 128);
		}
		if(strcmp(row[2], "C")==0) {
			sprintf(answer, "C) ");
			strcat(answer, row[3]);
			gtk_button_set_label(GTK_BUTTON(exam_answer_c), (const gchar*)answer);
			memset(answer, 0, 128);
		}
		if(strcmp(row[2], "D")==0) {
			sprintf(answer, "D) ");
			strcat(answer, row[3]);
			gtk_button_set_label(GTK_BUTTON(exam_answer_d), (const gchar*)answer);
			memset(answer, 0, 128);
		}
	}
}

int get_question_count() {
	sprintf(sql_select, "SELECT * FROM questions WHERE exam_id=%d", exam_obj.id);
	mysql_query(conn, sql_select);
	res = mysql_store_result(conn);
	int num_rows = mysql_num_rows(res);
	mysql_free_result(res);
	return num_rows;
}

void server_listener() {
	char recv_buffer[MSG_LEN] = {};
	char session_str[64];
    if(recv(c_server_socket, recv_buffer, MSG_LEN, 0) <=0) {
		printf("%s\n","Error");
	} else {
		session_id = atoi(recv_buffer);
		memset(recv_buffer, 0, MSG_LEN);
	}
	while (1) {
        int receive = recv(c_server_socket, recv_buffer, MSG_LEN, 0);
        if (receive > 0) {
			if(recv_buffer[0]=='!') {
				sprintf(sql_select, "SELECT student_id, SUM(score) AS total FROM responses WHERE session_id = %d GROUP BY(student_id) ORDER BY total DESC", session_id);
				if(mysql_query(conn, sql_select)) {
			    	fprintf(stderr, "%s\n", mysql_error(conn)); 
  				}
				res = mysql_store_result(conn);
				int y=1;
				while ((row = mysql_fetch_row(res)))
				{
					int x=0;
					GtkWidget *label_temp1 = gtk_label_new((const gchar*) row[x]);
					gtk_widget_set_name (label_temp1, (const gchar*)("text-white"));
					gtk_grid_attach(GTK_GRID(grid_exam_results), label_temp1, x, y, 1, 1);
					x = x + 1;
					GtkWidget *label_temp2 = gtk_label_new((const gchar*) row[x]);
					gtk_widget_set_name (label_temp2, (const gchar*)("text-white"));
					gtk_grid_attach(GTK_GRID(grid_exam_results), label_temp2, x, y, 1, 1);
					char temp[3];
					sprintf(temp, "%d", y);
					x = x + 1;
					GtkWidget *label_temp3 = gtk_label_new((const gchar*) temp);
					gtk_widget_set_name (label_temp3, (const gchar*)("text-white"));
					gtk_grid_attach(GTK_GRID(grid_exam_results), label_temp3, x, y, 1, 1);
					memset(temp, 0, 3);
					y = y + 1;
				}
				if(gtk_widget_is_visible(spinner_results)) {
					gtk_widget_hide(label_exam_question);
					gtk_widget_hide(spinner_results);
				} else
				{
					gtk_widget_hide(grid_exam_answers);
					gtk_widget_hide(label_exam_question);
					gtk_widget_hide(label_exam_question_number);
				}
				
				gtk_widget_show(scroll_exam);
				gtk_widget_show(label_results_announced);
				gtk_widget_show_all(grid_exam_results);
				gtk_widget_show(btn_finish);
			}

        } else if (receive == 0) {
			break;
        } else { 
            fprintf(stderr, "socket() failed: %s\n", strerror(errno));
        }
    }
}

void join_exam(GtkButton* b, int y) {
	int exam_id = atoi(gtk_label_get_text(GTK_LABEL(gtk_grid_get_child_at(grid_exams, 3, y))));
	printf("Joining exam: %d \n", exam_id);
	sprintf(sql_select, "SELECT ip_address, port_number FROM exams WHERE id=%d ", exam_id);
	exam_obj.id = exam_id;
	if(mysql_query(conn, sql_select)) {
    	fprintf(stderr, "%s\n", mysql_error(conn)); 
  	}
	
	res = mysql_store_result(conn);
	char exam_ip[32];
	int port;
	while ((row = mysql_fetch_row(res)))
	{
		sprintf(exam_ip, row[0]);
		port  = atoi(row[1]);
	}
	exam_ip[strlen(exam_ip)-1] = 0;
	
    char buffer[1024] = {0}; 
    if ((c_server_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) 
    { 
        printf("\n Socket creation error \n"); 
        return -1; 
    } 
   
    c_serv_addr.sin_family = AF_INET; 
    c_serv_addr.sin_port = htons(PORT); 
       
    // Convert IPv4 and IPv6 addresses from text to binary form 
    if(inet_pton(AF_INET, exam_ip, &c_serv_addr.sin_addr)<=0)  
    { 
        printf("\nInvalid address/ Address not supported \n"); 
        return -1; 
    } 
   
    if (connect(c_server_socket, (struct sockaddr *)&c_serv_addr, sizeof(c_serv_addr)) < 0) 
    { 
        printf("\nConnection Failed \n"); 
        return -1; 
    } 
	
	pthread_t server_listener_thread;
	if (pthread_create(&server_listener_thread, NULL, (void *) server_listener, NULL) != 0) {
        printf ("Create pthread error!\n");
        exit(EXIT_FAILURE);
    }

	char msg[128];
	sprintf(msg, "$|%s", user_obj.id);
	strcat(msg, "|");
	
	char len_str[2];
	sprintf(len_str, "%d", strlen(user_obj.full_name));
	strcat(msg, len_str);
	strcat(msg, "|");
	strncat(msg, user_obj.full_name, strlen(user_obj.full_name));
	printf("%s\n", msg);
	
	send(c_server_socket, msg, strlen(msg), 0);
    
	gtk_widget_hide(st_window_panel);

	gtk_label_set_text(GTK_LABEL(label_exam_course), (const gchar*)gtk_label_get_text(GTK_LABEL(gtk_grid_get_child_at(grid_exams, 1, y))));
	gtk_label_set_text(GTK_LABEL(label_exam_title), (const gchar*)gtk_label_get_text(GTK_LABEL(gtk_grid_get_child_at(grid_exams, 2, y))));
	gtk_label_set_text(GTK_LABEL(label_exam_professor), (const gchar*)gtk_label_get_text(GTK_LABEL(gtk_grid_get_child_at(grid_exams, 0, y))));
	
	get_next_question();
    
	gtk_widget_show(window_exam);
}

void on_exam_answer_a_clicked(GtkButton *b) {
	send_answer("A");
	if(current_question + 1 <= get_question_count()) {
		current_question = current_question + 1;
		get_next_question();
	} else {
		hide_exam_grid();
	}
	
}
void on_exam_answer_b_clicked(GtkButton *b) {
	send_answer("B");
	if(current_question + 1 <= get_question_count()) {
		current_question = current_question + 1;
		get_next_question();
	} else {
		hide_exam_grid();
	}
}
void on_exam_answer_c_clicked(GtkButton *b) {
	send_answer("C");
	if(current_question + 1 <= get_question_count()) {
		current_question = current_question + 1;
		get_next_question();
	} else {
		hide_exam_grid();
	}
}
void on_exam_answer_d_clicked(GtkButton *b) {
	send_answer("D");
	if(current_question + 1 <= get_question_count()) {
		current_question = current_question + 1;
		get_next_question();
	} else {
		hide_exam_grid();
	}
}
void on_btn_finish_clicked(GtkButton *b) {
	exit(EXIT_SUCCESS);
}

void send_answer(char *letter) {
	char answer[128];
	sprintf(answer, "@|%s|", user_obj.id);
	
	char exam_id_str[8];
	sprintf(exam_id_str, "%d", exam_obj.id);
	strncat(answer, exam_id_str, strlen(exam_id_str));
	strcat(answer, "|");
	
	strcat(answer, gtk_label_get_text(GTK_LABEL(label_exam_question_number)));
	strcat(answer, "|");
	
	strcat(answer, letter);
	
	send(c_server_socket, answer, strlen(answer), 0);
}

void hide_exam_grid() {
	gtk_widget_hide(grid_exam_answers);
	gtk_widget_hide(label_exam_question_number);
	gtk_label_set_text(GTK_LABEL(label_exam_question), (const gchar*)"You answered all questions!\nYou will get your results when the exam is finished...");
	gtk_widget_show(spinner_results);
}

void append_new_exam() {
	sprintf(sql_select, "SELECT id, title FROM exams WHERE prof_id='%s' AND id=%d", user_obj.id, exam_obj.id);
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

void update_exam_status(int status) {

	if(status == START) {
		
		GLOBAL_EXAM_STATUS = START;
		
		gtk_widget_set_sensitive(combo_start_quiz, FALSE);
		gtk_widget_set_sensitive(btn_start_exam, FALSE);
		//Create new exam session
		sprintf(sql_insert, "INSERT INTO sessions(started_at) VALUES(NOW())");
		if(mysql_query(conn, sql_insert)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
  		}
		session_id = mysql_insert_id(conn);
		
		char ip[20];
		get_ip_address(ip);
		ip[strcspn(ip, "\n")] = 0;
		if( ip[strlen(ip)-1] == '\n' )
    		ip[strlen(ip)-1] = 0;		
		//Update exam status and store server ip and port in the database
		sprintf(sql_update, "UPDATE exams SET status=1, ip_address='%s', port_number=%d WHERE id=%d", ip, PORT, exam_obj.id);
		if (mysql_query(conn, sql_update))
		{            
			mysql_errno(conn);         
		}           
		res = mysql_use_result(conn);
		mysql_free_result(res);
	} else if (status == FINISH) {
		//Finish this session
		sprintf(sql_insert, "UPDATE sessions SET finished_at=NOW() WHERE id=%d", session_id);
		if(mysql_query(conn, sql_insert)) {
			fprintf(stderr, "%s\n", mysql_error(conn));
  		}
		GLOBAL_EXAM_STATUS = START;
		sprintf(sql_update, "UPDATE exams SET status=0 WHERE id=%d", exam_obj.id);
		if (mysql_query(conn, sql_update))
		{            
			mysql_errno(conn);         
		}           
		res = mysql_use_result(conn);
		mysql_free_result(res);
	}
	
}

void on_btn_refresh_clicked (GtkButton* b) {
	if(gtk_grid_get_child_at(GTK_GRID(grid_exams), 0, 1)) {
		gtk_grid_remove_row(GTK_GRID(grid_exams), 0);
	} else
	{
		get_online_exams();
		gtk_widget_show_all(grid_exams);
	}
}

void on_btn_finish_exam_clicked(GtkButton *b) {
	update_exam_status(FINISH);
	StudentList *temp = head->next;
	while (temp!=NULL)
	{
		printf("%d\n", temp->fd);
		printf("%s\n", temp->ip);
		send(temp->fd, "!", 1, 0);
		temp = temp->next;
	}
	gtk_widget_hide(btn_start_exam);
	gtk_widget_show(label_exam_finished);
	gtk_widget_set_sensitive(combo_course, FALSE);
	gtk_widget_set_sensitive(btn_finish_exam, FALSE);
	close(s_server_fd);
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
    fclose(fp);
	strcpy(buf, ip);
	system("rm ip.txt");
	if (ip)
        free(ip);
}

void split_string(char buf[], char* array[]){
    int i = 0;
    char *p = strtok (buf, "|");

    while (p != NULL)
    {
        array[i++] = p;
        p = strtok (NULL, "|");
    }
}

int session_history_id;
int history_count = -1;
void on_combo_history_changed(GtkComboBox *c) {
	session_history_id = atoi(gtk_combo_box_get_active_id(combo_history));
	printf("%d\n", session_history_id);
	if(history_count!=-1) {
		for(int i=history_count; i>=1; i--) {
			gtk_grid_remove_row(GTK_GRID(grid_history), i);
		}
		gtk_widget_hide(grid_history);
	}
	
	sprintf(sql_select, "SELECT R.student_id, students.full_name, SUM(R.score) AS total, R.exam_id, R.session_id, S.started_at, E.title FROM responses R JOIN exams E ON R.exam_id=E.id JOIN students ON students.id=R.student_id JOIN sessions S ON S.id=R.session_id GROUP BY R.student_id, R.session_id HAVING (R.exam_id IN (SELECT exams.id FROM exams WHERE exams.prof_id='%s')) AND R.session_id=%d", user_obj.id, session_history_id);
	if(mysql_query(conn, sql_select)) {
    	fprintf(stderr, "%s\n", mysql_error(conn));
		return;
  	}
	res = mysql_store_result(conn);  
	history_count = mysql_num_rows(res);
	
	int y=1;
	while ((row = mysql_fetch_row(res)))
	{
		int x=0;
		gtk_grid_attach(GTK_GRID(grid_history), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_history), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_history), gtk_label_new((const gchar*) row[x]), x, y, 1, 1);
		y = y + 1;
	}
	mysql_free_result(res);
	gtk_widget_show_all(grid_history);
}

void get_history() {
	sprintf(sql_select, "SELECT R.student_id, SUM(R.score) AS total, R.exam_id, R.session_id, S.started_at, E.title, C.title FROM responses R JOIN exams E ON R.exam_id=E.id JOIN courses C ON E.course_id=C.id JOIN sessions S ON S.id=R.session_id GROUP BY R.student_id, R.session_id HAVING (R.exam_id IN (SELECT exams.id FROM exams WHERE exams.prof_id='%s'))", user_obj.id);
	if(mysql_query(conn, sql_select)) {
    	fprintf(stderr, "%s\n", mysql_error(conn));
		return;
  	}
	res = mysql_store_result(conn);
	
	while ((row = mysql_fetch_row(res)))
	{
		char temp[128];
		sprintf(temp, row[4]);
		strcat(temp, " ");
		strcat(temp, row[6]);
		strcat(temp, " ");
		strcat(temp, row[5]);
		gtk_list_store_insert_with_values(GTK_LIST_STORE(liststore4), NULL, -1, 0, row[3], 1, temp, -1);		
		memset(temp, 0, 128);
	}
	mysql_free_result(res);
}

void get_student_history() {
	sprintf(sql_select, "SELECT R.student_id, SUM(R.score) AS total, C.title, E.title, R.exam_id, R.session_id, S.started_at FROM responses R JOIN exams E ON R.exam_id=E.id JOIN courses C ON C.id=E.course_id JOIN sessions S ON S.id=R.session_id GROUP BY R.student_id, R.session_id HAVING R.student_id='%s'", user_obj.id);
	if(mysql_query(conn, sql_select)) {
    	fprintf(stderr, "%s\n", mysql_error(conn));
		return;
  	}
	res = mysql_store_result(conn);
	int y=1;
	while ((row = mysql_fetch_row(res)))
	{
		int x=0;
		gtk_grid_attach(GTK_GRID(grid_history_student), gtk_label_new((const gchar*) row[2]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_history_student), gtk_label_new((const gchar*) row[3]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_history_student), gtk_label_new((const gchar*) row[1]), x, y, 1, 1);
		x = x + 1;
		gtk_grid_attach(GTK_GRID(grid_history_student), gtk_label_new((const gchar*) row[6]), x, y, 1, 1);
		y = y + 1;
	}
	gtk_widget_show_all(grid_history_student);	
}

//////////////////// student password handler ///////////////////////////////


void on_st_current_id_changed(GtkEntry *ew){
	gtk_label_set_text(GTK_LABEL(st_update_error_label), (const gchar*) "");
	gtk_label_set_text(GTK_LABEL(st_update_success_label), (const gchar*) "");

	sprintf(user_current_id, "%s", gtk_entry_get_text(ew));
	
}

void on_st_current_pwd_changed(GtkEntry *er) {

	gtk_label_set_text(GTK_LABEL(st_update_error_label), (const gchar*) "");
	gtk_label_set_text(GTK_LABEL(st_update_success_label), (const gchar*) "");
		
	sprintf(user_current_password, "%s", gtk_entry_get_text(er));
}
		
void on_st_new_pwd_changed(GtkEntry *ep){
		
	sprintf(user_new_password, "%s", gtk_entry_get_text(ep));
	
}
void on_student_pwd_update_btn_clicked(GtkButton *e){

	char sql_request[1024];

	
	if(user_current_id[0]=='S' &&
	memcmp(user_id, user_current_id, sizeof(user_current_id))==0 &&
    memcmp(user_password, user_current_password, sizeof(user_current_password))==0){
		
		sprintf(sql_request, "UPDATE students SET password='%s' WHERE id='%s'", user_new_password, user_obj.id);

	if (mysql_query(conn, sql_request))
		{            
			mysql_errno(conn);   
			gtk_label_set_text(GTK_LABEL(st_update_error_label), (const gchar*) "Low internet connection...");      
		}        

		res = mysql_use_result(conn);
		mysql_free_result(res);
		gtk_label_set_text(GTK_LABEL(st_update_success_label), (const gchar*) "you have successfully modified");
		
	}
	else {
		gtk_label_set_text(GTK_LABEL(st_update_error_label), (const gchar*) "invalid type or wrong password *");
		
	}
	
}



//**************************** student password handler ********************//


//////////////////// professor password handler ///////////////////////////////

void on_pr_current_id_changed(GtkEntry *ew){
	gtk_label_set_text(GTK_LABEL(pr_update_error_label), (const gchar*) "");
	gtk_label_set_text(GTK_LABEL(pr_update_success_label), (const gchar*) "");

	sprintf(user_current_id, "%s", gtk_entry_get_text(ew));
}

void on_pr_current_pwd_changed(GtkEntry *er) {

	gtk_label_set_text(GTK_LABEL(pr_update_error_label), (const gchar*) "");
	gtk_label_set_text(GTK_LABEL(pr_update_success_label), (const gchar*) "");
		
	sprintf(user_current_password, "%s", gtk_entry_get_text(er));
}
		
void on_pr_new_pwd_changed(GtkEntry *ep){
		
	sprintf(user_new_password, "%s", gtk_entry_get_text(ep));
	
}
void on_professor_pwd_update_btn_clicked(GtkButton *e){

	if(user_current_id[0]=='P' &&
	memcmp(user_id, user_current_id, sizeof(user_current_id))==0 &&
    memcmp(user_password, user_current_password, sizeof(user_current_password))==0){
		
		sprintf(sql_select, "UPDATE professors SET password='%s' WHERE id='%s'", user_new_password, user_obj.id);

	if (mysql_query(conn, sql_select))
		{            
			mysql_errno(conn);   
			gtk_label_set_text(GTK_LABEL(pr_update_error_label), (const gchar*) "Low internet connection...");      
		}        

		res = mysql_use_result(conn);
		mysql_free_result(res);
		gtk_label_set_text(GTK_LABEL(pr_update_success_label), (const gchar*) "you have successfully modified");

	}
	else {
		gtk_label_set_text(GTK_LABEL(pr_update_error_label), (const gchar*) "invalid type or wrong password *");
		
	}
	
}
//**************************** student password handler ********************//