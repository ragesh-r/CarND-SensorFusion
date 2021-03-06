#include "kalman_filter.h"

using Eigen::MatrixXd;
using Eigen::VectorXd;

/* 
 * Please note that the Eigen library does not initialize 
 *   VectorXd or MatrixXd objects with zeros upon creation.
 */

KalmanFilter::KalmanFilter() {}

KalmanFilter::~KalmanFilter() {}

void KalmanFilter::Init(VectorXd &x_in, MatrixXd &P_in, MatrixXd &F_in,
                        MatrixXd &H_in, MatrixXd &R_in, MatrixXd &Q_in) {
  x_ = x_in;
  P_ = P_in;
  F_ = F_in;
  H_ = H_in;
  R_ = R_in;
  Q_ = Q_in;
}

void KalmanFilter::Predict() {
  /**
   * TODO: predict the state
   */
  x_ = F_ * x_;
  MatrixXd Ft = F_.transpose();
  P_ = F_ * P_ * Ft + Q_;
}

void KalmanFilter::Update(const VectorXd &z) {
  /**
   * TODO: update the state by using Kalman Filter equations
   */
  VectorXd y = z - H_ * x_;
  KalmanUpdate(y);
}

void KalmanFilter::UpdateEKF(const VectorXd &z) {
  /**
   * TODO: update the state by using Extended Kalman Filter equations
   */
  VectorXd hx = VectorXd(3);
  
  //states
  
  float px = x_(0);
  float py = x_(1);
  float vx = x_(2);
  float vy = x_(3);
  
  float ro = sqrt(px*px + py*py);
  //avoid division by 0
  if(fabs(ro) < 0.0001) {
    ro = 0.0001;
  }
    
  float theta = atan2(py, px);
  float ro_dot = (px*vx + py*vy) / ro;
  
  hx << ro, theta, ro_dot;
  VectorXd y = z - hx;
  
  while(y(1) > M_PI){
      y(1) -= 2 * M_PI;
    }

   while(y(1) < -M_PI){
      y(1) += 2 * M_PI;
    }
  
  KalmanUpdate(y);
}


void KalmanFilter::KalmanUpdate(const VectorXd &y) {
	MatrixXd Ht = H_.transpose();
	MatrixXd PHt = P_ * Ht;
    MatrixXd S = H_ * PHt + R_;
	MatrixXd Si = S.inverse();
	MatrixXd K = PHt * Si;

	//new estimate
	x_ = x_ + (K * y);
	long x_size = x_.size();
	MatrixXd I = MatrixXd::Identity(x_size, x_size);
	P_ = (I - K * H_) * P_;
}
