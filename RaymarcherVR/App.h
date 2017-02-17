#ifndef APP_H
#define APP_H 1
//Main app class singltone style
class MainApp{
public:
	static MainApp& getInstance()
	{
		static MainApp instance;
		return instance;
	}
	//App entry point
	void run(int argc, char * argv[]);
	//App terminate
	void terminate();

private:
	MainApp(){};
	~MainApp(){};
	
public:
	MainApp(MainApp const&) = delete;
	void operator=(MainApp const&) = delete;
};
#endif