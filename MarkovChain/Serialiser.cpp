template<class T>
class Serialiser {

public:
    virtual void serialise(double t, state_values<T> states) {
        std::cout << t;
        for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
        {
            std::cout << "," << it->second;
        }
        std::cout << "\n";
    }

    virtual void serialiseHeader(state_values<T> states) {
        std::cout << "t";
        for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
        {
            std::cout << "," << it->first;
        }
        std::cout << "\n";
    }

    virtual void serialiseFinally(double t, state_values<T> states) {}
};

template<class T>
class SerialiserFile : public Serialiser<T> {
private:
    std::ofstream mOutputfile;
    
public:
    
    SerialiserFile(std::string filename) {
        mOutputfile.open(filename);
    }

    virtual void serialise(double t, state_values<T> states) {
        mOutputfile << t;

        typename std::map<std::string,T>::iterator it;

        for(it=states.begin(); it != states.end(); it++) {
            mOutputfile << "," << it->second;
        }
        mOutputfile << "\n";
        mOutputfile.flush();
    }

    virtual void serialiseHeader(state_values<T> states) {
        mOutputfile << "t";
        for (typename state_values<T>::iterator it = states.begin(); it != states.end(); it++)
        {
            mOutputfile << "," << it->first;
        }
        mOutputfile << "\n";
        mOutputfile.flush();
    }

    virtual void serialiseFinally(double t, state_values<T> states) {}
};

template<class T>
class SerialiserFileFinalState : public SerialiserFile<T> {

public:
    SerialiserFileFinalState(std::string filename) : SerialiserFile<T>(filename) {}
    
    virtual void serialise(double t, state_values<T> states) {}

    void serialiseFinally(double t, state_values<T> states) {
        SerialiserFile<T>::serialise(t, states);
    }
};

template<class T>
class SerialiserPredefinedTimes : public Serialiser<T> {
private:
    std::vector<double> mSerialiseTimes;
    state_values<T> mLastState;

public:
    SerialiserPredefinedTimes(std::vector<double> serialiseTimes) : mSerialiseTimes(serialiseTimes) {}
    virtual void serialise(double t, state_values<T> states) {
        double next_time = *mSerialiseTimes.begin();

        while (t > next_time) {
            Serialiser<T>::serialise(next_time, mLastState);
            mSerialiseTimes.erase(mSerialiseTimes.begin());
            next_time = *mSerialiseTimes.begin();
        }
        mLastState = states;
    }
};

template<class T>
class SerialiserPredefinedTimesFile : public SerialiserFile<T> {
private:
    std::vector<double> mSerialiseTimes;
    state_values<T> mLastState;
    double mLastT;

public:
    SerialiserPredefinedTimesFile(std::vector<double> serialiseTimes, std::string filename) : SerialiserFile<T>(filename), mSerialiseTimes(serialiseTimes) {}
    
    virtual void serialise(double t, state_values<T> states) {
        double next_time = *mSerialiseTimes.begin();
        while (t > next_time && !mSerialiseTimes.empty()) {
            //Interpolate between the points.
            state_values<T> interpolated_states;
            for (auto &p : states)
            {
                double slope = ( (double) p.second - mLastState[p.first]) / (t - mLastT);
                interpolated_states[p.first] = slope * (next_time - mLastT) + mLastState[p.first];
            }
            SerialiserFile<T>::serialise(next_time, interpolated_states);
            mSerialiseTimes.erase(mSerialiseTimes.begin());
            next_time = *mSerialiseTimes.begin();
        }
        mLastState = states;
        mLastT = t;
    }

    virtual void serialiseFinally(double t, state_values<T> states)
    {
        serialise(t, states);
    }
};