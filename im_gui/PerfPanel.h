
class PerfPanel {

public:
	PerfPanel();
	~PerfPanel() {}
	void render();
	void tick(float dt);
private:
	float _timer;
	float _data[16];
	int _num;
};