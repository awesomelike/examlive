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

//GTK student main menu
GtkWidget *st_window_panel;
GtkWidget *st_name;
GtkWidget *st_id;
GtkWidget *label_button_student;
GtkWidget *grid_exams;
GtkWidget *join_button[10];

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

	//declare variable student panel
	st_window_panel = GTK_WIDGET(gtk_builder_get_object(builder, "st_window_panel"));
	st_id = GTK_WIDGET(gtk_builder_get_object(builder, "st_id"));
	st_name = GTK_WIDGET(gtk_builder_get_object(builder, "st_name"));
	label_button_student = GTK_WIDGET(gtk_builder_get_object(builder, "leave_button_student"));
	grid_exams = GTK_WIDGET(gtk_builder_get_object(builder, "grid_exams"));
	

	//These lines used to connect CSS
    provider = gtk_css_provider_new();
    display = gdk_display_get_default();
    screen = gdk_display_get_default_screen (display);
    gtk_style_context_add_provider_for_screen (screen, GTK_STYLE_PROVIDER(provider), GTK_STYLE_PROVIDER_PRIORITY_USER);
    gtk_css_provider_load_from_path(GTK_CSS_PROVIDER(provider),"style/style.css",NULL);
}