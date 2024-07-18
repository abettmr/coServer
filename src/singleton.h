#ifndef __SINGLETON__
#define __SINGLETON__

template <typename T> class singleton {
private:
  singleton(const singleton &anothor);
  singleton &operator=(const singleton<T> &);

protected:
  singleton(){};
  virtual ~singleton() {}

public:
  static T& getInst() {
    static T instance{}; // this is important.
    return instance;
  }
};

#endif