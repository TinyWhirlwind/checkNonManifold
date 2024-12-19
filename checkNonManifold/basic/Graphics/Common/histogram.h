
#ifndef _HISTOGRAM_
#define _HISTOGRAM_

#include <vector>
#include <algorithm>
#include <assert.h>
#include <string>




/**
 * Histogram.
 *
 * This class implements a single-value histogram.
 */
template <class ScalarType> 
class Histogram 
{

// public data members
public:

	//! Counters for bins.
	std::vector <int> H; 
	
	//! Range for bins.
	std::vector <ScalarType> R;

	//! Minimum value.
	ScalarType minv;

	//! Maximum value.
	ScalarType maxv;

	//! Number of intervals.
	int n;

	//! Number of accumulated samples.
	int cnt;

	//! Average.
	ScalarType avg;

	//! Root mean square.
	ScalarType rms;

// public methods
public:

	/** 
	 * Set the histogram values.
	 *
	 * This method is used to correctly initialize the bins of the histogram.
	 */
	void SetRange(ScalarType _minv, ScalarType _maxv, int _n);

	/**
	 * Set the histogram values.
	 *
	 * This method is used to correctly initialize the bins of the histogram.
	 * The \a gamma parameter is applied to modify the ranges of the bins.
	 */
	void SetRange(ScalarType _minv, ScalarType _maxv, int _n, ScalarType gamma);

	/** 
	 * Returns the index of the bin which contains a given value.
	 */
	int Interize(ScalarType val);

	/** 
	 * Add a new value to the histogram.
	 *
	 * The statistics related to the histogram data (average, RMS, etc.) are 
	 * also updated.
	 */
	void Add(ScalarType v);

	/** 
	 * Returns the value corresponding to a given percentile of the data.
	 *
	 * The percentile range between 0 and 1.
	 */
	ScalarType Percentile(ScalarType frac) const;
	
	//! Returns the average of the data.
	ScalarType Avg(){ return avg/cnt;}
	
	//! Returns the Root Mean Square of the data.
	ScalarType RMS(){ return (float)sqrt(rms/double(cnt));}
	
	//! Returns the variance of the data.
	ScalarType Variance(){ return fabs(rms/cnt-Avg()*Avg());}
	
	//! Returns the standard deviation of the data.
	ScalarType StandardDeviation(){ return sqrt(Variance());}

	//! Dump the histogram to a file.
	void FileWrite(const std::string &filename);

	//! Reset histogram data.
	void Clear();
};

template <class ScalarType> 
void Histogram<ScalarType>::Clear()
{
	H.clear();
	R.clear();
	cnt=0;
	avg=0;
	rms=0;
	n=0;
	minv=0;
	maxv=1;
}


template <class ScalarType> 
void Histogram<ScalarType>::SetRange(ScalarType _minv, ScalarType _maxv, int _n)
{
	// reset data
	Clear();

	// set bins
	minv=_minv;maxv=_maxv;n=_n;
	H.resize(n+1);
	fill(H.begin(),H.end(),0);
	R.resize(n+1);
	ScalarType dlt=(maxv-minv)/n;
	for(int i=0; i<n+1; ++i)
		R[i]=minv+dlt*i;
}


template <class ScalarType> 
void Histogram<ScalarType>::SetRange(ScalarType _minv, ScalarType _maxv, int _n, ScalarType gamma)
{
	// reset data
	Clear();
	
	minv=_minv;maxv=_maxv;n=_n;
	H.resize(n+1);
	fill(H.begin(),H.end(),0);
	R.resize(n+1);
	
	double dlt=(maxv-minv);
	for(int i=0;i<n+1;++i)
		R[i]=(ScalarType)(minv+dlt*pow(ScalarType(i)/n,gamma));
}


template <class ScalarType> 
int Histogram<ScalarType>::Interize(ScalarType val) 
{
	int pos = lower_bound(R.begin(),R.end(),val) - R.begin() - 1;
	if (pos>n) pos=n;
	return pos;
}


template <class ScalarType> 
void Histogram<ScalarType>::Add(ScalarType v)
{
	int pos= lower_bound(R.begin(),R.end(),v)-R.begin()-1;

	if(pos>=0 && pos<=n)
	{
		++H[pos];
		++cnt;
		avg+=v;
		rms += v*v;
	}
}


template <class ScalarType> 
void Histogram<ScalarType>::FileWrite(const std::string &filename)
{
	FILE *fp;
	fp=fopen(filename.c_str(),"w");
	
	for(unsigned int i=0; i<H.size(); i++)
		fprintf (fp,"%12.8lf , %12.8lf \n",R[i],double(H[i])/cnt);

	fclose(fp);
}


template <class ScalarType> 
ScalarType Histogram<ScalarType>::Percentile(ScalarType frac) const
{
	if(H.size()==0 && R.size()==0) 
		return 0;
	
	// check percentile range
	assert(frac >= 0 && frac <= 1);
	
	ScalarType sum=0,partsum=0;
	int isum=0;
	int i;

	for(i=0;i<n+1;i++)
	{
		sum+=H[i]; 
		isum+=H[i];
	}
	
	// check 
	assert(isum==cnt);
	assert(sum==cnt);

	sum*=frac;
	for(i=0; i<n; i++)
	{
		partsum+=H[i];
		if(partsum>=sum) break;
	}
	
	return R[i+1];
}

typedef Histogram<double> Histogramd ;
typedef Histogram<float> Histogramf ;



#endif  