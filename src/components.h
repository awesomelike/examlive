#include<gtk/gtk.h>
#include<gtk/gtkx.h>

GtkWidget	*login_window;
GtkWidget	*login_fixed;
GtkWidget	*login_image_1;
GtkWidget	*login_button;
GtkWidget	*login_username;
GtkWidget	*login_password;
GtkBuilder	*builder; 
GtkWidget   *login_label_error;

//GTK professor main panel
GtkWidget *pr_window_panel;
GtkWidget *start_btn;
GtkWidget *pr_swap_panel;
GtkWidget *label_prof_name;
GtkWidget *label_prof_id;
GtkWidget *login_spinner;

//GTK professor create quiz
GtkWidget *entry_exam_title;
GtkWidget *combo_course;
GtkWidget *entry_combo;
GtkWidget *liststore2;
GtkWidget *label_num_questions;
GtkWidget *entry_question;
GtkWidget *grid_create_answers;
GtkWidget *entry_answer_a;
GtkWidget *entry_answer_b;
GtkWidget *entry_answer_c;
GtkWidget *entry_answer_d;
GtkWidget *radio_a;
GtkWidget *radio_b;
GtkWidget *radio_c;
GtkWidget *radio_d;
GtkWidget *btn_add_question;
GtkWidget *btn_save_exam;
GtkWidget *spinner_add_question;

//GTK professor start quiz
GtkWidget *combo_start_quiz;
GtkWidget *entry_combo_start_quiz;
GtkWidget *liststore3;
GtkWidget *btn_start_exam;
GtkWidget *grid_student_results;
GtkWidget *btn_finish_exam;
GtkWidget *label_exam_finished;

//GTK student main menu
GtkWidget *st_window_panel;
GtkWidget *st_name;
GtkWidget *st_id;
GtkWidget *label_button_student;
GtkWidget *grid_exams;
GtkWidget *join_button[10];

//GTK student exam
GtkWidget *window_exam;
GtkWidget *label_exam_course;
GtkWidget *label_exam_title;
GtkWidget *label_exam_professor;
GtkWidget *grid_exam_answers;
GtkWidget *exam_answer_a;
GtkWidget *exam_answer_b;
GtkWidget *exam_answer_c;
GtkWidget *exam_answer_d;
GtkWidget *label_exam_question;
GtkWidget *label_exam_question_number;
GtkWidget *spinner_results;
GtkWidget *grid_exam_results;
GtkWidget *scroll_exam;
GtkWidget *label_results_announced;
GtkWidget *btn_finish;

GtkWidget *grid_history_student;

//GTK Student Password update panel
GtkWidget *st_current_id;
GtkWidget *st_current_pwd;
GtkWidget *st_new_pwd;
GtkWidget *student_pwd_update_btn;
GtkWidget *st_update_error_label;
GtkWidget *st_update_success_label;

//GTK Professor History page
GtkWidget *combo_history;
GtkWidget *entry_combo_history;
GtkWidget *grid_history;
GtkWidget *liststore4;

//GTK Student Password update panel
GtkWidget *pr_current_id;
GtkWidget *pr_current_pwd;
GtkWidget *pr_new_pwd;
GtkWidget *professor_pwd_update_btn;
GtkWidget *pr_update_error_label;
GtkWidget *pr_update_success_label;
//CSS values
GtkCssProvider  *provider;
GdkScreen       *screen;
GdkDisplay      *display;

void initialize_components() {
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
	login_spinner = GTK_WIDGET(gtk_builder_get_object(builder, "login_spinner"));

	//declare variable professor panel
	start_btn = GTK_WIDGET(gtk_builder_get_object(builder, "start_btn"));
	pr_swap_panel = GTK_WIDGET(gtk_builder_get_object(builder, "swaping_panel"));
	pr_window_panel=  GTK_WIDGET(gtk_builder_get_object(builder, "pr_window_panel"));
	label_prof_name = GTK_WIDGET(gtk_builder_get_object(builder, "label_prof_name"));
	label_prof_id = GTK_WIDGET(gtk_builder_get_object(builder, "label_prof_id"));

	//declare variable professor create quiz
	entry_exam_title = GTK_WIDGET(gtk_builder_get_object(builder, "entry_exam_title"));
	combo_course = GTK_WIDGET(gtk_builder_get_object(builder, "combo_course"));
	entry_combo = GTK_WIDGET(gtk_builder_get_object(builder, "entry_combo"));
	liststore2 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore2"));
	entry_question = GTK_WIDGET(gtk_builder_get_object(builder, "entry_question"));
	label_num_questions = GTK_WIDGET(gtk_builder_get_object(builder, "label_num_questions"));
	grid_create_answers = GTK_WIDGET(gtk_builder_get_object(builder, "grid_create_answers"));
	entry_answer_a = GTK_WIDGET(gtk_builder_get_object(builder, "entry_answer_a"));
	entry_answer_b = GTK_WIDGET(gtk_builder_get_object(builder, "entry_answer_b"));
	entry_answer_c = GTK_WIDGET(gtk_builder_get_object(builder, "entry_answer_c"));
	entry_answer_d = GTK_WIDGET(gtk_builder_get_object(builder, "entry_answer_d"));
	radio_a = GTK_WIDGET(gtk_builder_get_object(builder, "radio_a"));
	radio_b = GTK_WIDGET(gtk_builder_get_object(builder, "radio_b"));
	radio_c = GTK_WIDGET(gtk_builder_get_object(builder, "radio_c"));
	radio_d = GTK_WIDGET(gtk_builder_get_object(builder, "radio_d"));
	btn_add_question = GTK_WIDGET(gtk_builder_get_object(builder, "btn_add_question"));
	btn_save_exam = GTK_WIDGET(gtk_builder_get_object(builder, "btn_save_exam"));
	spinner_add_question = GTK_WIDGET(gtk_builder_get_object(builder, "spinner_add_question"));

	//declare variable professor start quiz
	combo_start_quiz = GTK_WIDGET(gtk_builder_get_object(builder, "combo_start_quiz"));
	entry_combo_start_quiz = GTK_WIDGET(gtk_builder_get_object(builder, "entry_combo_start_quiz"));
	liststore3 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore3"));
	btn_start_exam = GTK_WIDGET(gtk_builder_get_object(builder, "btn_start_exam"));
	grid_student_results = GTK_WIDGET(gtk_builder_get_object(builder, "grid_student_results"));
	btn_finish_exam = GTK_WIDGET(gtk_builder_get_object(builder, "btn_finish_exam"));
	label_exam_finished = GTK_WIDGET(gtk_builder_get_object(builder, "label_exam_finished"));

	//declare variable student panel
	st_window_panel = GTK_WIDGET(gtk_builder_get_object(builder, "st_window_panel"));
	st_id = GTK_WIDGET(gtk_builder_get_object(builder, "st_id"));
	st_name = GTK_WIDGET(gtk_builder_get_object(builder, "st_name"));
	label_button_student = GTK_WIDGET(gtk_builder_get_object(builder, "leave_button_student"));
	grid_exams = GTK_WIDGET(gtk_builder_get_object(builder, "grid_exams"));
	
	grid_history_student = GTK_WIDGET(gtk_builder_get_object(builder, "grid_history_student"));

	//declare variables exam panel
	window_exam = GTK_WIDGET(gtk_builder_get_object(builder, "window_exam"));
	label_exam_course = GTK_WIDGET(gtk_builder_get_object(builder, "label_exam_course"));
	label_exam_title = GTK_WIDGET(gtk_builder_get_object(builder, "label_exam_title"));
	label_exam_professor = GTK_WIDGET(gtk_builder_get_object(builder, "label_exam_professor"));
	grid_exam_answers = GTK_WIDGET(gtk_builder_get_object(builder, "grid_exam_answers"));
	exam_answer_a = GTK_WIDGET(gtk_builder_get_object(builder, "exam_answer_a"));
	exam_answer_b = GTK_WIDGET(gtk_builder_get_object(builder, "exam_answer_b"));
	exam_answer_c = GTK_WIDGET(gtk_builder_get_object(builder, "exam_answer_c"));
	exam_answer_d = GTK_WIDGET(gtk_builder_get_object(builder, "exam_answer_d"));
	label_exam_question = GTK_WIDGET(gtk_builder_get_object(builder, "label_exam_question"));
	label_exam_question_number = GTK_WIDGET(gtk_builder_get_object(builder, "label_exam_question_number"));
	spinner_results = GTK_WIDGET(gtk_builder_get_object(builder, "spinner_results"));
	grid_exam_results = GTK_WIDGET(gtk_builder_get_object(builder, "grid_exam_results"));
	scroll_exam = GTK_WIDGET(gtk_builder_get_object(builder, "scroll_exam"));
	label_results_announced = GTK_WIDGET(gtk_builder_get_object(builder, "label_results_announced"));
	btn_finish = GTK_WIDGET(gtk_builder_get_object(builder, "btn_finish"));

	//declare student Password update panel
	st_current_id= GTK_WIDGET(gtk_builder_get_object(builder, "st_current_id"));
	st_current_pwd = GTK_WIDGET(gtk_builder_get_object(builder, "st_current_pwd"));
	st_new_pwd= GTK_WIDGET(gtk_builder_get_object(builder, "st_new_pwd"));
	student_pwd_update_btn = GTK_WIDGET(gtk_builder_get_object(builder, "student_pwd_update_btn"));
	st_update_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "st_update_error_label"));
	st_update_success_label = GTK_WIDGET(gtk_builder_get_object(builder, "st_update_success_label"));

	//declare student Password update panel
	pr_current_id= GTK_WIDGET(gtk_builder_get_object(builder, "pr_current_id"));
	pr_current_pwd = GTK_WIDGET(gtk_builder_get_object(builder, "pr_current_pwd"));
	pr_new_pwd= GTK_WIDGET(gtk_builder_get_object(builder, "pr_new_pwd"));
	professor_pwd_update_btn = GTK_WIDGET(gtk_builder_get_object(builder, "professor_pwd_update_btn"));
	pr_update_error_label = GTK_WIDGET(gtk_builder_get_object(builder, "pr_update_error_label"));
	pr_update_success_label = GTK_WIDGET(gtk_builder_get_object(builder, "pr_update_success_label"));

	//declare professor history page vars
	combo_history = GTK_WIDGET(gtk_builder_get_object(builder, "combo_history"));
	entry_combo_history = GTK_WIDGET(gtk_builder_get_object(builder, "entry_combo_history"));
	grid_history = GTK_WIDGET(gtk_builder_get_object(builder, "grid_history"));
	liststore4 = GTK_LIST_STORE(gtk_builder_get_object(builder, "liststore4"));

	//These lines used to connect CSS
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),"style/style.css",NULL);
}