Timeline ball1=new Timeline(new KeyFrame(Duration.millis(2),e-> {
	//pane2.getChildren().add(point1);
	/*
	if(point1.getCenterX()<=95.0 && point1.getCenterY()==480.0 ) {
		if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
		point1.setCenterX(point1.getCenterX()+1.0);
	}*/
	if(point1.getCenterX()<=130.0 &&point1.getCenterX()>50.0 && point1.getCenterY()==480.0) {
		if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
		if(point1.getCenterX()==130.0) {
			point1.setCenterX(310.0);
			point1.setCenterY(520.0);
		}
		else point1.setCenterX(point1.getCenterX()+1.0);
		//point2.setCenterY(point1.getCenterY()+1);
	}
	else if(point1.getCenterX()<330.0 && point1.getCenterY()==520.0) point1.setCenterX(point1.getCenterX()+1.0);
	else if(point1.getCenterX()==330.0 && point1.getCenterY()>65.0) point1.setCenterY(point1.getCenterY()-1.0);
	else if(point1.getCenterX()<=460.0 &&point1.getCenterX()>320.0&& point1.getCenterY()==65.0) {//shift_left1:460,20,45,90
		if(point1.getCenterX()==460.0) point1.setCenterX(505.0);
		else point1.setCenterX(point1.getCenterX()+1.0);
	}
	else if(point1.getCenterX()<1280.0  && point1.getCenterX()>500.0 &&point1.getCenterY()==65.0)point1.setCenterX(point1.getCenterX()+1.0);
	else if(point1.getCenterX()==1280.0 && point1.getCenterY()<105.0&&point1.getCenterY()>20.0) point1.setCenterY(point1.getCenterY()+1.0);
	else if(point1.getCenterX()<=1320.0 && point1.getCenterX()>1270.0 &&point1.getCenterY()==105.0) {
		if(point1.getCenterX()==1320.0) {point1.setCenterX(1360.0);point1.setCenterY(135.0);}
		point1.setCenterX(point1.getCenterX()+1.0);
	}
	else if(point1.getCenterX()<1380.0&&point1.getCenterX()>20.0 && point1.getCenterY()==135.0) point1.setCenterX(point1.getCenterX()+1.0);
	else if(point1.getCenterX()==1380 && point1.getCenterY()>10.0) point1.setCenterY(point1.getCenterY()-1.0);
	else if(point1.getCenterX()>10.0 && point1.getCenterY()==10.0)point1.setCenterX(point1.getCenterX()-1.0);
	else if(point1.getCenterX()==10.0 && point1.getCenterY()<480.0)  point1.setCenterY(point1.getCenterY()+1.0);
	else if(point1.getCenterX()<=30.0 && point1.getCenterY()==480.0)  {
		if(point1.getCenterX()==30.0) {pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
		else point1.setCenterX(point1.getCenterX()+1.0);
	}
	else ;//{pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
}));
ball1.setCycleCount(Timeline.INDEFINITE);
ball1.play();