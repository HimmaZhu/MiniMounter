/**
 * @file SyntheticSequenceGenerator_.cpp
 * @brief mex interface for cv::bgsegm::SyntheticSequenceGenerator
 * @ingroup bgsegm
 * @author Amro
 * @date 2018
 */
#include "mexopencv.hpp"
#include "opencv2/bgsegm.hpp"
using namespace std;
using namespace cv;
using namespace cv::bgsegm;

// Persistent objects
namespace {
/// Last object id to allocate
int last_id = 0;
/// Object container
map<int,Ptr<SyntheticSequenceGenerator> > obj_;
}

/**
 * Main entry called from Matlab
 * @param nlhs number of left-hand-side arguments
 * @param plhs pointers to mxArrays in the left-hand-side
 * @param nrhs number of right-hand-side arguments
 * @param prhs pointers to mxArrays in the right-hand-side
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[])
{
    // Check the number of arguments
    nargchk(nrhs>=2 && nlhs<=2);

    // Argument vector
    vector<MxArray> rhs(prhs, prhs+nrhs);
    int id = rhs[0].toInt();
    string method(rhs[1].toString());

    // constructor call
    if (method == "new") {
        nargchk(nrhs>=4 && (nrhs%2)==0 && nlhs<=1);
        double amplitude = 2.0;
        double wavelength = 20.0;
        double wavespeed = 0.2;
        double objspeed = 6.0;
        for (int i=4; i<nrhs; i+=2) {
            string key(rhs[i].toString());
            if (key == "Amplitude")
                amplitude = rhs[i+1].toDouble();
            else if (key == "WaveLength")
                wavelength = rhs[i+1].toDouble();
            else if (key == "WaveSpeed")
                wavespeed = rhs[i+1].toDouble();
            else if (key == "ObjSpeed")
                objspeed = rhs[i+1].toDouble();
            else
                mexErrMsgIdAndTxt("mexopencv:error",
                    "Unrecognized option %s", key.c_str());
        }
        Mat background(rhs[2].toMat(CV_8U)),
            object(rhs[3].toMat(CV_8U));
        obj_[++last_id] = createSyntheticSequenceGenerator(background, object,
            amplitude, wavelength, wavespeed, objspeed);
        plhs[0] = MxArray(last_id);
        mexLock();
        return;
    }

    // Big operation switch
    Ptr<SyntheticSequenceGenerator> obj = obj_[id];
    if (obj.empty())
        mexErrMsgIdAndTxt("mexopencv:error", "Object not found id=%d", id);
    if (method == "delete") {
        nargchk(nrhs==2 && nlhs==0);
        obj_.erase(id);
        mexUnlock();
    }
    else if (method == "clear") {
        nargchk(nrhs==2 && nlhs==0);
        obj->clear();
    }
    else if (method == "load") {
        nargchk(nrhs>=3 && (nrhs%2)==1 && nlhs==0);
        string objname;
        bool loadFromString = false;
        for (int i=3; i<nrhs; i+=2) {
            string key(rhs[i].toString());
            if (key == "ObjName")
                objname = rhs[i+1].toString();
            else if (key == "FromString")
                loadFromString = rhs[i+1].toBool();
            else
                mexErrMsgIdAndTxt("mexopencv:error",
                    "Unrecognized option %s", key.c_str());
        }
        /*
        obj_[id] = (loadFromString ?
            Algorithm::loadFromString<SyntheticSequenceGenerator>(rhs[2].toString(), objname) :
            Algorithm::load<SyntheticSequenceGenerator>(rhs[2].toString(), objname));
        */
        ///*
        // HACK: workaround for missing SyntheticSequenceGenerator::create()
        FileStorage fs(rhs[2].toString(), FileStorage::READ +
            (loadFromString ? FileStorage::MEMORY : 0));
        if (!fs.isOpened())
            mexErrMsgIdAndTxt("mexopencv:error", "Failed to open file");
        FileNode fn(objname.empty() ? fs.getFirstTopLevelNode() : fs[objname]);
        if (fn.empty())
            mexErrMsgIdAndTxt("mexopencv:error", "Failed to get node");
        obj->read(fn);
        //*/
    }
    else if (method == "save") {
        nargchk(nrhs==3 && nlhs==0);
        obj->save(rhs[2].toString());
    }
    else if (method == "empty") {
        nargchk(nrhs==2 && nlhs<=1);
        plhs[0] = MxArray(obj->empty());
    }
    else if (method == "getDefaultName") {
        nargchk(nrhs==2 && nlhs<=1);
        plhs[0] = MxArray(obj->getDefaultName());
    }
    else if (method == "getNextFrame") {
        nargchk(nrhs==2 && nlhs<=2);
        Mat frame, gtMask;
        obj->getNextFrame(frame, gtMask);
        plhs[0] = MxArray(frame);
        if (nlhs > 1)
            plhs[1] = MxArray(gtMask);
    }
    else
        mexErrMsgIdAndTxt("mexopencv:error",
            "Unrecognized operation %s", method.c_str());
}
