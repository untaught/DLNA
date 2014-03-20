#ifndef CONTROLLER_H
#define CONTROLLER_H

class Controller
{
public:
	virtual void OnHttpResponseReceived(LPSTR response, BOOL xml)=0;
};

#endif