template<class T>
class Serialiser {

public:
    virtual void serialise(double t, state_values<T> states) {
        std::cout << t << std::endl;
    }

    virtual void serialiseHeader(state_values<T> states) {
        std::cout << "t";
        for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
        {
            std::cout << "," << it->first;
        }
        std::cout << "\n";
    }
};

template<class T>
class SerialiserFile : public Serialiser<T> {
private:
    std::ofstream mOutputfile;
    
public:
    
    SerialiserFile(std::string filename) {
        mOutputfile.open(filename);
    }

    void serialise(double t, state_values<T> states) {
        mOutputfile << t;

        typename std::map<std::string,T>::iterator it;

        for(it=states.begin(); it != states.end(); it++) {
            mOutputfile << "," << it->second;
        }
        mOutputfile << "\n";
    }

    void serialiseHeader(state_values<T> states) {
        mOutputfile << "t";
        for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
        {
            mOutputfile << "," << it->first;
        }
        mOutputfile << "\n";
    }
};