
#include <thread>
#include <iostream>
#include <vector>
#include <mutex>

using int32 = __int32;
using namespace std;

vector<int32> v;
mutex m;
/*
   1.c++ stl에서 지원하는 함수들은 보통 단일스레드 환경 기준으로 설계가 되어있다.
   그렇기 때문에 예를들어 동적으로 vector가 정수를 push받으면서 계속해서 vector이 사이즈를 동적할당하는 과정에 메모리에 영역을 삭제중에 
   다른 스레드가 끼어들어 그 영역에 삭제를 한번더내리거나 만약 그 부분에 push를하게 되면 crush가 발생할수있다.  
   결론 mvsc에서 지원하거나 c++에서 지원 되는 함수가 멀티스레드환경에서 아무 문제없이 동작할꺼라 생각하지말고 의심 할것

   가장 기본적인 lock
   Mutual Exclusive(상호배타적)
   일단 기본적으로 Mutext는 재귀적으로 호출할수없다(연속해서 lock 또는 unlock불가능).
   recursive mutex 는 가능 개발을하다보면 이 뮤텍스를 연속으로 사용하고싶을때가있다. 
   function에서 락을 걸고 다른 function에가서 처리중에 뮤텍스를 만나게되는경우. 
   개발자 입장에선 만약 사용가능하다면 편하겟지만 코드 전체적으로 볼때는 세밀한 주의가 필요한부분이 생기기 되겠다.
   또한 lock사용시 주의할점이 unlock을 반드시 해주어야한다.
   누가안하겠냐 싶겠지만 만약 반복문안에서 조건문으로 break로 unlock이 실행되기전에 빠저나가버리면 프로그램이 영원히 실행 될 것이다.
   그리고 코드가 길어지면 if else안에 계속 lock unlock을 찾는 것도 일이다. 그래서 나온 코딩법중에
    RAII(Resourece Acquistion Is Initizlization) 
    ex) 래퍼클래스에서 생성자에 lock 소멸자에 unlock을 배치를 하여 객체 소멸시 unlock되도록 설정
        
*/
template<typename T>
class LockGuard { //뮤텍스를 자동으로 관리하기위한 래퍼클래스 자동으로 언락을 하기위함. RAII(Resourece Acquistion Is Initizlization) 준수
                  //std표준(lock_guard<std::mutex> *function(arg))에도 있다. 연습용삼아 만든 짭 lockguard
public:
    LockGuard(T& m) {
        _mutex = &m;
        _mutex->lock();
    }
    ~LockGuard() {
        _mutex->lock();
    }
private:
    T* _mutex;

};

void Push() {
    for (int32 i = 0; i < 10000; i++)
    {
       //m.lock();
        LockGuard<std::mutex> lockGuard(m);
       // unique_lock<std::mutex> uniqueLock(m, std::defer_lock);//unique_lock처럼 선언만 해놓고 인자에따라 유동적으로 사용가능한 락가드도 존재
       //uniqueLock.lock();                                     //추가적으로 받는 인자를 잘 알고있어야할듯.

        v.push_back(i);
        //m.unlock();
    }
}

int main()
{
    thread  t1(Push);
    thread  t2(Push);

    t1.join();
    t2.join();

    std::cout << v.size()<<'\n';
}
