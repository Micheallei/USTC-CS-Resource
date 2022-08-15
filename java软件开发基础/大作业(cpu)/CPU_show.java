package cpu;

import java.io.File;
import javafx.application.Application;
import javafx.geometry.Insets;
import javafx.geometry.Pos;
import javafx.collections.FXCollections;
import javafx.collections.ObservableList;
import javafx.scene.Scene;
import javafx.scene.control.* ;
import javafx.stage.FileChooser;
import javafx.scene.control.ComboBox;
import javafx.scene.layout.*;
import javafx.scene.paint.Color;
import javafx.scene.shape.*;
import javafx.scene.text.Text; 
import javafx.scene.text.Font; 
import javafx.scene.text.FontWeight;
import javafx.scene.text.FontPosture;
import javafx.util.Duration;
import javafx.animation.*;
import javafx.stage.Stage;


public class CPU_show extends Application{
	public void start(Stage primaryStage) {
		
		CPU_logic cpu=new CPU_logic();

		HBox hBox1 = new HBox(15);	//设置PC显示
		hBox1.setPadding(new Insets(15,15,15,15));
		hBox1.setStyle("-fx-background-color:lightgrey");
		Label label_PC = new Label("PC              ");
		label_PC.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		TextField PC_tf = new TextField();
		
		PC_tf.setText("0");
		PC_tf.setTooltip(new Tooltip("0-255"));
		PC_tf.setAlignment(Pos.BOTTOM_LEFT);
		PC_tf.setEditable(true);
		hBox1.getChildren().addAll(label_PC,PC_tf);
		
		HBox hBox2 = new HBox(15);		//设置Instruction显示
		hBox2.setPadding(new Insets(15,15,15,15));
		hBox2.setStyle("-fx-background-color:lightgrey");
		Label label_Instr = new Label("Instruction");
		label_Instr.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		TextField Instr_tf = new TextField();
		Instr_tf.setAlignment(Pos.BOTTOM_LEFT);
		Instr_tf.setEditable(false);
		Instr_tf.setText(cpu.Instruction_memory[0]);
		hBox2.getChildren().addAll(label_Instr,Instr_tf);
		
		
		//设置Registers显示
		String[] reg_names = {"$t0","$t1","$t2","$t3","$t4","$t5","$t6","$t7",
				"$s0","$s1","$s2","$s3","$s4","$s5","$s6","$s7"};
		
		Text text1 = new Text(5,10,"$t0");
		ComboBox<String> cbo = new ComboBox<>();//下拉框创建
		cbo.setPrefWidth(60);
		cbo.setValue("$t0");
		cbo.setStyle("-fx-background-color:white");
		cbo.setTooltip(new Tooltip("Registers"));
        
		ObservableList<String> items=FXCollections.observableArrayList(reg_names);
		cbo.getItems().addAll(items);
		
		StackPane paneForComboBox = new StackPane();
		paneForComboBox.getChildren().addAll(cbo,text1);
        		
		Label Reg_value = new Label("Value");
		Reg_value.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		Reg_value.setPadding(new Insets(10,10,10,10));
		TextField R_value_tf = new TextField();
		R_value_tf.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.Reg_Parse("$t0")]));
		R_value_tf.setAlignment(Pos.BOTTOM_LEFT);
		R_value_tf.setEditable(true);
		
		cbo.getSelectionModel().selectedIndexProperty().addListener(
                (ov, value, new_value) -> {        
                	System.out.println(cpu.Registers[cpu.Reg_Parse(cbo.getValue())]);
                	text1.setText(reg_names[new_value.intValue()]);
                	R_value_tf.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.Reg_Parse(cbo.getValue())]));
                });
		
		HBox paneForRegisters=new HBox();
		paneForRegisters.setPadding(new Insets(15,15,15,15));
		paneForRegisters.setStyle("-fx-background-color:lightgrey");
		Text label_Reg = new Text("Register");
		label_Reg.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 13));
		paneForRegisters.getChildren().addAll(label_Reg,paneForComboBox,Reg_value,R_value_tf);
		
				
		//Data memory address和Value显示设置
		HBox hBox3 = new HBox(15);
		hBox3.setPadding(new Insets(15,15,15,15));
		hBox3.setStyle("-fx-background-color:lightgrey");
		HBox hBox4 = new HBox(15);
		hBox4.setPadding(new Insets(15,15,15,15));
		hBox4.setStyle("-fx-background-color:lightgrey");
		
		Label lb_data_addr = new Label("Data_mem addr");
		Label lb_data = new Label("Data_mem value");
		lb_data_addr.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		lb_data.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		Text addr = new Text(5,5,"0");
		TextField data_tf = new TextField();
		data_tf.setText(cpu.binaryStringToDecimalString(cpu.Data_memory[0]));
		data_tf.setAlignment(Pos.BOTTOM_LEFT);
		data_tf.setEditable(true);
		
		Slider siHorizontal = new Slider();
		siHorizontal.setShowTickLabels(true);
		siHorizontal.setShowTickMarks(true);
		siHorizontal.setMin(0);
		siHorizontal.setMax(63);
		siHorizontal.setValue(0);		
		siHorizontal.valueProperty().addListener(ov ->{
			data_tf.setText(cpu.binaryStringToDecimalString(cpu.Data_memory[(new Double(Math.floor(siHorizontal.getValue()))).intValue()]));
			addr.setText((new Double(Math.floor(siHorizontal.getValue()))).intValue()+"");
		});
		
		hBox3.getChildren().addAll(lb_data,data_tf);
		hBox4.getChildren().addAll(lb_data_addr,siHorizontal,addr);
		
		
		//Button设置
		Button btSetValue = new Button("set value");
		btSetValue.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		Button btStart = new Button("start");
		btStart.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		btSetValue.setOnAction(e->{//根据文本框中的值，设置PC,Register,Data memory Value，Instruction
			cpu.PC=cpu.DecimalStringTobinaryString(PC_tf.getText()).substring(2).concat("00");
			Instr_tf.setText(cpu.Instruction_memory[Integer.valueOf(PC_tf.getText()).intValue()]);
			cpu.Registers[cpu.Reg_Parse(cbo.getValue())]=cpu.DecimalStringTobinaryString(R_value_tf.getText());
			cpu.Data_memory[(new Double(Math.floor(siHorizontal.getValue()))).intValue()]=cpu.DecimalStringTobinaryString(data_tf.getText());		
		});
		
		
		btStart.setOnAction(e->{//运行cpu一个周期，并在界面上显示相应值
			
			cpu.run_one();
			PC_tf.setText(cpu.binaryStringToDecimalString("00".concat(cpu.PC.substring(0,30))));
			Instr_tf.setText(cpu.Instruction_memory[Integer.valueOf(PC_tf.getText()).intValue()]);
			R_value_tf.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.Reg_Parse(cbo.getValue())]));
			data_tf.setText(cpu.binaryStringToDecimalString(cpu.Data_memory[(new Double(Math.floor(siHorizontal.getValue()))).intValue()]));
			SecondStage_start(cpu);
		});
		
		HBox paneForButtons = new HBox(15);
		paneForButtons.setPadding(new Insets(15,15,15,15));
		paneForButtons.setStyle("-fx-background-color:lightgrey");
		paneForButtons.getChildren().addAll(btSetValue,btStart);
		
		
		Button btLoad1 = new Button("Load Data_mem");
		btLoad1.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
		btLoad1.setOnAction(e->{//读取文件并将其加载进入Data memory，只能读.txt文件
                FileChooser fileChooser = new FileChooser();
                FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("TXT files (*.txt)", "*.txt");
                fileChooser.getExtensionFilters().add(extFilter);
                File file = fileChooser.showOpenDialog(primaryStage);
                if (file!=null) {
                	cpu.load_file(file,1);                	
                	data_tf.setText(cpu.binaryStringToDecimalString(cpu.Data_memory[(new Double(Math.floor(siHorizontal.getValue()))).intValue()]));
                }
            });
        
        Button btLoad2 = new Button("Load Instr_mem");
        btLoad2.setFont(Font.font("Courier", FontWeight.BOLD, FontPosture.ITALIC, 18));
        btLoad2.setOnAction(e->{//读取文件并将其加载进入Instruction memory，只能读.txt文件;加载之后，PC变为0，Instruction也相应变化
                FileChooser fileChooser = new FileChooser();
                FileChooser.ExtensionFilter extFilter = new FileChooser.ExtensionFilter("TXT files (*.txt)", "*.txt");
                fileChooser.getExtensionFilters().add(extFilter);
                File file = fileChooser.showOpenDialog(primaryStage);
                if (file!=null) {
                	cpu.load_file(file,0);
                	cpu.PC="00000000000000000000000000000000";
                	Instr_tf.setText(cpu.Instruction_memory[0]);
                }
            });
       
      
        HBox paneForButtonLoads = new HBox(15);
		paneForButtonLoads.setPadding(new Insets(15,15,15,15));
		paneForButtonLoads.setStyle("-fx-background-color:lightgrey");
		paneForButtonLoads.getChildren().addAll(btLoad1,btLoad2);
		
		
		GridPane pane=new GridPane();
		pane.setAlignment(Pos.CENTER);
		pane.setPadding(new Insets(15,15,15,15));
		pane.setHgap(5.5);
		pane.setVgap(5.5);
		
		pane.addColumn(0,hBox1,hBox2,paneForRegisters,hBox3,hBox4,paneForButtons,paneForButtonLoads);
		
		Scene scene = new Scene(pane,450,500);
		primaryStage.setTitle("CPU panel");
		primaryStage.setWidth(460);
		primaryStage.setHeight(570);
		primaryStage.setResizable(false);
		primaryStage.setScene(scene);
		primaryStage.show();
		
	}

	
	
	public void SecondStage_start(CPU_logic cpu) {
		Pane pane = new Pane();
		
		Text text_pc = new Text(40,485,"PC");
		text_pc.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_instr = new Text(225,520,"Instruction\n[31:0]");
		text_instr.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_instr_addr = new Text(135,460,"Read\naddress");
		text_instr_addr.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_instr_mem = new Text(135,550,"Instruction\nmemory");
		text_instr_mem.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_reg_raddr1 = new Text(605,450,"Read\nregister1");
		text_reg_raddr1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_reg_raddr2 = new Text(605,490,"Read\nregister2");
		text_reg_raddr2.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_reg_waddr = new Text(605,560,"Write register");
		text_reg_waddr.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_reg_wdata = new Text(605,600,"Write Data");
		text_reg_wdata.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_reg = new Text(660,530,"Registers");
		text_reg.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_reg_rdata1 = new Text(740,450,"Read\ndata1");
		text_reg_rdata1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));

		Text text_reg_rdata2 = new Text(740,555,"Read\ndata2");
		text_reg_rdata2.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_ALU = new Text(975,525,"ALU");
		text_ALU.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_ALU_result = new Text(1010,530,"ALU\nresult");
		text_ALU_result.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_dmem_addr = new Text(1155,540,"Address");
		text_dmem_addr.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_dmem_wdata = new Text(1155,630,"Write\ndata");
		text_dmem_wdata.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_dmem = new Text(1240,600,"Data\nmemory");
		text_dmem.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_dmem_rdata = new Text(1290,530,"Read\ndata");
		text_dmem_rdata.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux1_0 = new Text(505,546,"0");
		text_mux1_0.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux1_1 = new Text(505,605,"1");
		text_mux1_1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux1 = new Text(517,560,"M\nu\nx");
		text_mux1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux2_0 = new Text(865,556,"0");
		text_mux2_0.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux2_1 = new Text(865,615,"1");
		text_mux2_1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux2 = new Text(877,570,"M\nu\nx");
		text_mux2.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux3_0 = new Text(1385,535,"0"); 
		text_mux3_0.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux3_1 = new Text(1385,625,"1");
		text_mux3_1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux3 = new Text(1397,565,"M\nu\nx");
		text_mux3.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux4_0 = new Text(1225,110,"0");
		text_mux4_0.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux4_1 = new Text(1225,200,"1");
		text_mux4_1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux4 = new Text(1237,135,"M\nu\nx");
		text_mux4.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux5_0 = new Text(1325,110,"0");
		text_mux5_0.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux5_1 = new Text(1325,200,"1");
		text_mux5_1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_mux5 = new Text(1337,135,"M\nu\nx");
		text_mux5.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_sign = new Text(740,680,"Sign\nextend");
		text_sign.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_shift1 = new Text(467,60,"Sign\nextend");
		text_shift1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_shift2 = new Text(867,250,"Sign\nextend");
		text_shift2.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_4 = new Text(145,274,"4");
		text_4.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_add1 = new Text(220,225,"Add");
		text_add1.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Text text_add2 = new Text(990,200,"Add");
		text_add2.setFont(Font.font("Courier",FontWeight.BOLD,FontPosture.ITALIC,15));
		
		Rectangle pc = new Rectangle(30,430,40,100);
		pc.setStroke(Color.LIGHTSLATEGRAY );
		pc.setFill(Color.INDIANRED  );
		
				
		Rectangle Instr_mem = new Rectangle(130,430,180,180);
		Instr_mem.setStroke(Color.BLACK);
		Instr_mem.setFill(Color.RED);
		
		Rectangle Registers = new Rectangle(600,430,180,180);
		Registers.setStroke(Color.BLACK);
		Registers.setFill(Color.LIGHTPINK );
	
		Rectangle Data_mem = new Rectangle(1150,480,180,180);
		Data_mem.setStroke(Color.BLACK);
		Data_mem.setFill(Color.RED);		
		
		Rectangle mux1 = new Rectangle(500,530,40,90);
		mux1.setArcHeight(30);
		mux1.setArcWidth(50);		
		mux1.setStroke(Color.ORANGE);
		mux1.setFill(Color.AQUA );
		
		Rectangle mux2 = new Rectangle(860,540,40,90);
		mux2.setArcHeight(30);
		mux2.setArcWidth(50);
		mux2.setStroke(Color.ORANGE);
		mux2.setFill(Color.AQUA);
		
		Rectangle mux3 = new Rectangle(1380,515,40,125);
		mux3.setArcHeight(30);
		mux3.setArcWidth(50);
		mux3.setStroke(Color.ORANGE);
		mux3.setFill(Color.AQUA);
		
		
		Rectangle mux4 = new Rectangle(1220,90,40,125);
		mux4.setArcHeight(30);
		mux4.setArcWidth(50);
		mux4.setStroke(Color.ORANGE);
		mux4.setFill(Color.AQUA);
		
		Rectangle mux5 = new Rectangle(1320,90,40,125);
		mux5.setArcHeight(30);
		mux5.setArcWidth(50);
		mux5.setStroke(Color.ORANGE);
		mux5.setFill(Color.AQUA);
		
		Rectangle Sign_extend = new Rectangle(730,630,60,120);
		Sign_extend.setArcHeight(100);
		Sign_extend.setArcWidth(60);
		Sign_extend.setStroke(Color.BLACK);
		Sign_extend.setFill(Color.GOLD );
		
		Rectangle Shift_left_1 = new Rectangle(460,20,45,90);
		Shift_left_1.setArcHeight(100);
		Shift_left_1.setArcWidth(60);
		Shift_left_1.setStroke(Color.BLACK);
		Shift_left_1.setFill(Color.GOLD );
		
		Rectangle Shift_left_2 = new Rectangle(860,210,45,90);
		Shift_left_2.setArcHeight(100);
		Shift_left_2.setArcWidth(60);
		Shift_left_2.setStroke(Color.BLACK);
		Shift_left_2.setFill(Color.GOLD );
		
		
		Polygon ALU = new Polygon();
		ALU.setStroke(Color.ORANGE);
		ALU.setFill(Color.HONEYDEW );
		Double[] ALU_points= {950.0,430.0,950.0,500.0,970.0,520.0,950.0,540.0,950.0,610.0,1050.0,550.0,1050.0,490.0};
		ObservableList<Double> list1=FXCollections.observableArrayList(ALU_points);
		ALU.getPoints().addAll(list1);
		
		
		Polygon Add1 = new Polygon();
		Add1.setStroke(Color.ORANGE);
		Add1.setFill(Color.HONEYDEW );
		Double[] Add1_points= {190.0,130.0,190.0,200.0,210.0,220.0,190.0,240.0,190.0,310.0,260.0,260.0,260.0,180.0};
		ObservableList<Double> list2=FXCollections.observableArrayList(Add1_points);
		Add1.getPoints().addAll(list2);
		
		
		Polygon Add2 = new Polygon();
		Add2.setStroke(Color.ORANGE);
		Add2.setFill(Color.HONEYDEW );
		Double[] Add2_points= {950.0,110.0,950.0,180.0,970.0,200.0,950.0,220.0,950.0,290.0,1050.0,230.0,1050.0,170.0};
		ObservableList<Double> list3=FXCollections.observableArrayList(Add2_points);
		Add2.getPoints().addAll(list3);

		
		Polyline line1 = new Polyline();
		line1.setStroke(Color.BLACK);
		line1.setStrokeWidth(3);
		Double[] line1_points= {30.0,480.0,10.0,480.0,10.0,10.0,1380.0,10.0,1380.0,135.0,1360.0,135.0};
		ObservableList<Double> list4=FXCollections.observableArrayList(line1_points);
		line1.getPoints().addAll(list4);
		
		Polyline line2 = new Polyline();
		line2.setStroke(Color.BLACK);
		line2.setStrokeWidth(3);
		Double[] line2_points= {70.0,480.0,130.0,480.0};
		ObservableList<Double> list5=FXCollections.observableArrayList(line2_points);
		line2.getPoints().addAll(list5);
		
		Polyline line3 = new Polyline();
		line3.setStroke(Color.BLACK);
		line3.setStrokeWidth(3);
		Double[] line3_points= {70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0};
		ObservableList<Double> list6=FXCollections.observableArrayList(line3_points);
		line3.getPoints().addAll(list6);
		
		Polyline line4 = new Polyline();
		line4.setStroke(Color.BLACK);
		line4.setStrokeWidth(3);
		Double[] line4_points= {160.0,275.0,190.0,275.0};
		ObservableList<Double> list7=FXCollections.observableArrayList(line4_points);
		line4.getPoints().addAll(list7);
		
		Polyline line5 = new Polyline();
		line5.setStroke(Color.BLACK);
		line5.setStrokeWidth(3);
		Double[] line5_points= {310.0,520.0,330.0,520.0,330.0,65.0,1280.0,65.0,1280.0,105.0,1320.0,105.0};
		ObservableList<Double> list8=FXCollections.observableArrayList(line5_points);
		line5.getPoints().addAll(list8);
		
		Polyline line6 = new Polyline();
		line6.setStroke(Color.BLACK);
		line6.setStrokeWidth(3);
		Double[] line6_points= {260.0,190.0,360.0,190.0,360.0,120.0,600.0,120.0,600.0,65.0};
		ObservableList<Double> list9=FXCollections.observableArrayList(line6_points);
		line6.getPoints().addAll(list9);
		
		Polyline line7 = new Polyline();
		line7.setStroke(Color.BLACK);
		line7.setStrokeWidth(3);
		Double[] line7_points= {260.0,190.0,820.0,190.0,820.0,145.0,950.0,145.0};
		ObservableList<Double> list10=FXCollections.observableArrayList(line7_points);
		line7.getPoints().addAll(list10);
		
		Polyline line8 = new Polyline();
		line8.setStroke(Color.BLACK);
		line8.setStrokeWidth(3);
		Double[] line8_points= {260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0};
		ObservableList<Double> list11=FXCollections.observableArrayList(line8_points);
		line8.getPoints().addAll(list11);
		
		Polyline line9 = new Polyline();
		line9.setStroke(Color.BLACK);
		line9.setStrokeWidth(3);
		Double[] line9_points= {1050.0,200.0,1220.0,200.0};
		ObservableList<Double> list12=FXCollections.observableArrayList(line9_points);
		line9.getPoints().addAll(list12);
		
		Polyline line10 = new Polyline();
		line10.setStroke(Color.BLACK);
		line10.setStrokeWidth(3);
		Double[] line10_points= {1260.0,200.0,1320.0,200.0};
		ObservableList<Double> list13=FXCollections.observableArrayList(line10_points);
		line10.getPoints().addAll(list13);
		
		Polyline line11 = new Polyline();
		line11.setStroke(Color.BLACK);
		line11.setStrokeWidth(3);
		Double[] line11_points= {310.0,520.0,370.0,520.0,370.0,450.0,950.0,450.0};
		ObservableList<Double> list14=FXCollections.observableArrayList(line11_points);
		line11.getPoints().addAll(list14);
		
		Polyline line12 = new Polyline();
		line12.setStroke(Color.BLACK);
		line12.setStrokeWidth(3);
		Double[] line12_points= {310.0,520.0,370.0,520.0,370.0,490.0,600.0,490.0};
		ObservableList<Double> list15=FXCollections.observableArrayList(line12_points);
		line12.getPoints().addAll(list15);
		
		Polyline line13 = new Polyline();
		line13.setStroke(Color.BLACK);
		line13.setStrokeWidth(3);
		Double[] line13_points= {310.0,520.0,370.0,520.0,370.0,490.0,460.0,490.0,460.0,545.0,500.0,545.0};
		ObservableList<Double> list16=FXCollections.observableArrayList(line13_points);
		line13.getPoints().addAll(list16);
		
		Polyline line14 = new Polyline();
		line14.setStroke(Color.BLACK);
		line14.setStrokeWidth(3);
		Double[] line14_points= {310.0,520.0,370.0,520.0,370.0,605.0,500.0,605.0};
		ObservableList<Double> list17=FXCollections.observableArrayList(line14_points);
		line14.getPoints().addAll(list17);
		
		Polyline line15 = new Polyline();
		line15.setStroke(Color.BLACK);
		line15.setStrokeWidth(3);
		Double[] line15_points= {540.0,570.0,600.0,570.0};
		ObservableList<Double> list18=FXCollections.observableArrayList(line15_points);
		line15.getPoints().addAll(list18);
		
		Polyline line16 = new Polyline();
		line16.setStroke(Color.BLACK);
		line16.setStrokeWidth(3);
		Double[] line16_points= {310.0,520.0,370.0,520.0,370.0,690.0,830.0,690.0,830.0,255.0,950.0,255.0};
		ObservableList<Double> list19=FXCollections.observableArrayList(line16_points);
		line16.getPoints().addAll(list19);
		
		Polyline line17 = new Polyline();
		line17.setStroke(Color.BLACK);
		line17.setStrokeWidth(3);
		Double[] line17_points= {780.0,555.0,860.0,555.0};
		ObservableList<Double> list20=FXCollections.observableArrayList(line17_points);
		line17.getPoints().addAll(list20);
		
		Polyline line18 = new Polyline();
		line18.setStroke(Color.BLACK);
		line18.setStrokeWidth(3);
		Double[] line18_points= {310.0,520.0,370.0,520.0,370.0,690.0,830.0,690.0,830.0,615.0,860.0,615.0};
		ObservableList<Double> list21=FXCollections.observableArrayList(line18_points);
		line18.getPoints().addAll(list21);
		
		Polyline line19 = new Polyline();
		line19.setStroke(Color.BLACK);
		line19.setStrokeWidth(3);
		Double[] line19_points= {780.0,555.0,800.0,555.0,800.0,650.0,1150.0,650.0};
		ObservableList<Double> list22=FXCollections.observableArrayList(line19_points);
		line19.getPoints().addAll(list22);
		
		Polyline line20 = new Polyline();
		line20.setStroke(Color.BLACK);
		line20.setStrokeWidth(3);
		Double[] line20_points= {1050.0,530.0,1380.0,530.0};
		ObservableList<Double> list23=FXCollections.observableArrayList(line20_points);
		line20.getPoints().addAll(list23);
		
		Polyline line21 = new Polyline();
		line21.setStroke(Color.BLACK);
		line21.setStrokeWidth(3);
		Double[] line21_points= {1050.0,530.0,1100.0,530.0,1100.0,700.0,1350.0,700.0,1350.0,625.0,1380.0,625.0};
		ObservableList<Double> list24=FXCollections.observableArrayList(line21_points);
		line21.getPoints().addAll(list24);
		
		Polyline line22 = new Polyline();
		line22.setStrokeWidth(3);
		line22.setStroke(Color.BLACK );
		Double[] line22_points= {1420.0,595.0,1440.0,595.0,1440.0,780.0,560.0,780.0,560.0,595.0,600.0,595.0};
		ObservableList<Double> list25=FXCollections.observableArrayList(line22_points);
		line22.getPoints().addAll(list25);
		
		Polyline line23 = new Polyline();
		line23.setStroke(Color.BLACK);
		line23.setStrokeWidth(3);
		Double[] line23_points= {900.0,575.0,950.0,575.0};
		ObservableList<Double> list26=FXCollections.observableArrayList(line23_points);
		line23.getPoints().addAll(list26);
		
				
		Pane pane2=new Pane();//放球
		Pane pane3 = new Pane();//画轨迹
		pane.getChildren().addAll(pc,Instr_mem,Registers,ALU,Data_mem,mux1,mux2,mux3,mux4,mux5,
				Sign_extend,Shift_left_1,Shift_left_2,Add1,Add2,line1,line2,line3,line4,line5,
				line6,line7,line8,line9,line10,line11,line12,line13,line14,line15,line16,line17,
				line18,line19,line20,line21,line22,line23,text_pc,text_instr,text_instr_addr,
				text_instr_mem,text_reg_raddr1,text_reg_raddr2,text_reg_waddr,text_reg_wdata,
				text_reg,text_reg_rdata1,text_reg_rdata2,text_ALU,text_ALU_result,text_dmem_addr,
				text_dmem_wdata,text_dmem,text_dmem_rdata,text_mux1_0,text_mux1_1,text_mux1,
				text_mux2_0,text_mux2_1,text_mux2,text_mux3_0,text_mux3_1,text_mux3,
				text_mux4_0,text_mux4_1,text_mux4,text_mux5_0,text_mux5_1,text_mux5,
				text_sign,text_shift1,text_shift2,text_4,text_add1,text_add2,pane2,pane3);
	
		pane.setOnMousePressed(e->{
			if(text_pc.getText().equals("PC")) {
				text_pc.setText(cpu.binaryStringToDecimalString("00".concat(cpu.PC.substring(0,30))));
				pc.setFill(Color.WHITE);
			}
			else {text_pc.setText("PC");pc.setFill(Color.INDIANRED  );}
			if(text_instr_addr.getText().equals("Read\naddress")) {
				Instr_mem.setFill(Color.WHITE);
				text_instr_addr.setText("\n"+cpu.binaryStringToDecimalString("00".concat(cpu.PC_pre.substring(0,30))));
			}
			else {
				Instr_mem.setFill(Color.RED);
				text_instr_addr.setText("Read\naddress");
			}
			
			if(text_reg.getText().equals("Registers")) {
				if(text_reg_raddr1.getText().equals("Read\nregister1")) {
					Data_mem.setFill(Color.WHITE);
					Registers.setFill(Color.WHITE );
					switch(cpu.instruction.substring(0,6)) {
					case "000000"://add
						text_reg_raddr1.setText(String.valueOf(cpu.rs));
						text_reg_raddr2.setText(String.valueOf(cpu.rt));
						text_reg_waddr.setText("\n"+String.valueOf(cpu.rd));
						text_reg_rdata1.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rs]));
						text_reg_rdata2.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						text_reg_wdata.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rd]));
						text_ALU_result.setText("  "+cpu.binaryStringToDecimalString(cpu.Registers[cpu.rd]));
						break;
					case "001000"://addi
						text_reg_raddr1.setText(String.valueOf(cpu.rs));
						text_reg_waddr.setText("\n"+String.valueOf(cpu.rt));
						text_reg_rdata1.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rs]));
						text_reg_wdata.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						text_ALU_result.setText("  "+cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						break;
					case "100011"://lw
						text_reg_raddr1.setText(String.valueOf(cpu.rs));
						text_reg_waddr.setText("\n"+String.valueOf(cpu.rt));
						text_reg_rdata1.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rs]));
						text_reg_wdata.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						text_dmem_rdata.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						text_ALU_result.setText("  "+cpu.binaryStringToDecimalString(cpu.addr));
						text_dmem_addr.setText(cpu.binaryStringToDecimalString("00".concat(cpu.addr.substring(0,30))));
						break;
					case "101011"://sw
						text_reg_raddr1.setText(String.valueOf(cpu.rs));
						text_reg_raddr2.setText(String.valueOf(cpu.rt));
						text_reg_rdata1.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rs]));
						text_reg_rdata2.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						text_ALU_result.setText("  "+cpu.binaryStringToDecimalString(cpu.addr));
						text_dmem_addr.setText(cpu.binaryStringToDecimalString("00".concat(cpu.addr.substring(0,30))));
						text_dmem_wdata.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						break;
					case "000100"://beq
						text_reg_raddr1.setText(String.valueOf(cpu.rs));
						text_reg_raddr2.setText(String.valueOf(cpu.rt));
						text_reg_rdata1.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rs]));
						text_reg_rdata2.setText(cpu.binaryStringToDecimalString(cpu.Registers[cpu.rt]));
						break;
					}
				}
				else {
					Data_mem.setFill(Color.RED);
					Registers.setFill(Color.LIGHTPINK );
					text_reg_raddr1.setText("Read\nregister1");
					text_reg_raddr2.setText("Read\nregister2");
					text_reg_waddr.setText("Write register");
					text_reg_wdata.setText("Write data");
					text_reg_rdata1.setText("Read\ndata1");
					text_reg_rdata2.setText("Read\ndata2");
					text_ALU_result.setText("ALU\nresult");
					text_dmem_addr.setText("Address");
					text_dmem_wdata.setText("Write\ndata");
					text_dmem_rdata.setText("Read\ndata");
				}
			}
		});
		
		Circle point1 = new Circle(70,480,5);
		Circle point2 = new Circle(370,490,5);
		Circle point3 = new Circle(95,480,5);
		Circle point4 = new Circle(370,520,5);
		Circle point5 = new Circle(820,145,5);
		point1.setFill(Color.CHOCOLATE);
		point1.setStroke(Color.BISQUE);
		
		point2.setFill(Color.CHOCOLATE);
		point2.setStroke(Color.BISQUE);
		
		point3.setFill(Color.CHOCOLATE);
		point3.setStroke(Color.BISQUE);
		
		point4.setFill(Color.CHOCOLATE);
		point4.setStroke(Color.BISQUE);
		
		point5.setFill(Color.CHOCOLATE);
		point5.setStroke(Color.BISQUE);

		pane2.getChildren().add(point1);//1,3,4,2
		
		Timeline ball1;
		Timeline ball2;
		Timeline ball3;
		Timeline ball4;
		Timeline ball5;
		switch(cpu.instruction.substring(0,6)) {
		case "000000"://add指令
			 ball1=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				//pane2.getChildren().add(point1);
				if(point1.getCenterX()<=95.0 && point1.getCenterY()==480.0 ) {
					if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=130.0 && point1.getCenterY()==480.0) {
					if(point1.getCenterX()==130.0) {
						point1.setCenterX(310.0);
						point1.setCenterY(520.0);
					}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<370.0 && point1.getCenterY()==520.0) printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()>450.0 && point1.getCenterX()==370.0)  {
					if(point1.getCenterY()==520.0) pane2.getChildren().add(point4);
					if(point1.getCenterY()==490.0) pane2.getChildren().add(point2);
					printTrack(0,pane3,point1,3);
				}
				else if(point1.getCenterX()<=600.0 && point1.getCenterY()==450.0) {
					if(point1.getCenterX()==600.0) {point1.setCenterX(780.0);point1.setCenterY(450.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=950.0&& point1.getCenterY()==450.0) {
					if(point1.getCenterX()==950.0) {point1.setCenterX(1050.0);point1.setCenterY(530.0);}
					else printTrack(0,pane3,point1,0); 
				}
				else if(point1.getCenterX()<1100.0 && point1.getCenterY()==530.0) printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()<700.0 && point1.getCenterX()==1100.0) printTrack(0,pane3,point1,2);
				else if(point1.getCenterX()<1350.0&&point1.getCenterX()>1090.0&& point1.getCenterY()==700.0) printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()>625.0 &&point1.getCenterY()<720.0&&point1.getCenterX()==1350.0 ) printTrack(0,pane3,point1,3);
				else if(point1.getCenterX()<=1380&& point1.getCenterX()>1340.0&&point1.getCenterY()==625.0) {
					if(point1.getCenterX()==1380.0) {point1.setCenterX(1420.0);point1.setCenterY(595.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<1440.0&&point1.getCenterX()>1380.0&& point1.getCenterY()==595.0)printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()<780.0 && point1.getCenterX()==1440.0) printTrack(0,pane3,point1,2);
				else if(point1.getCenterX()>560.0&& point1.getCenterY()==780.0) printTrack(0,pane3,point1,1);
				else if(point1.getCenterY()>595.0 && point1.getCenterX()==560.0) printTrack(0,pane3,point1,3);
				else if(point1.getCenterX()<=600.0&& point1.getCenterY()==595.0) {
					if(point1.getCenterX()==600.0) {pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
					else printTrack(0,pane3,point1,0);
				}
				else ;
			}));
			ball1.setCycleCount(Timeline.INDEFINITE);
			ball1.play();
			
			ball2=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {
					if(point2.getCenterX()<=600.0 && point2.getCenterY()==490.0 ) {//780.0,555.0,860.0,555.0
						if(point2.getCenterX()==600.0) {point2.setCenterX(780.0);point2.setCenterY(555.0);}
						else printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()<=860.0 && point2.getCenterY()==555.0) {//900.0,575.0,950.0,575.0
						if(point2.getCenterX()==860.0) {point2.setCenterX(900.0);point2.setCenterY(575.0);}
						else printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()<=950.0 && point2.getCenterY()==575.0) {//900.0,575.0,950.0,575.0
						if(point2.getCenterX()==950.0) {pane2.getChildren().remove(2);point2.setCenterX(0.0);point2.setCenterY(0.0);}
						else printTrack(1,pane3,point2,0);
					}
					else ;
				}
			}));
			ball2.setCycleCount(Timeline.INDEFINITE);
			ball2.play();
					
			ball3=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point1.getCenterX()>=95.0||point1.getCenterX()==0.0) {//70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0
					if(point3.getCenterX()==95.0 && point3.getCenterY()>165.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=190.0&&point3.getCenterX()>20.0 && point3.getCenterY()==165.0) {//260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0
						if(point3.getCenterX()==190.0) {point3.setCenterX(260.0);point3.setCenterY(190.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<820.0&&point3.getCenterX()>20.0 && point3.getCenterY()==190.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==820.0 && point3.getCenterY()>105.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=1220.0 &&point3.getCenterX()>20.0&& point3.getCenterY()==105.0) {//1260.0,200.0,1320.0,200.0
						if(point3.getCenterX()==1220.0) {point3.setCenterX(1260.0);point3.setCenterY(200.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<=1320.0&&point3.getCenterX()>20.0 && point3.getCenterY()==200.0) {//30.0,480.0,10.0,480.0,10.0,10.0,1380.0,10.0,1380.0,135.0,1360.0,135.0
						if(point3.getCenterX()==1320.0) {point3.setCenterX(1360.0);point3.setCenterY(135.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<1380.0&&point3.getCenterX()>20.0 && point3.getCenterY()==135.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==1380 && point3.getCenterY()>10.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()>10.0 && point3.getCenterY()==10.0)printTrack(2,pane3,point3,1);
					else if(point3.getCenterX()==10.0 && point3.getCenterY()<480.0)  printTrack(2,pane3,point3,2);
					else if(point3.getCenterX()<=30.0 && point3.getCenterY()==480.0)  {
						if(point3.getCenterX()==30.0) {pane2.getChildren().remove(0);point3.setCenterX(0.0);point3.setCenterY(0.0);}
						else printTrack(2,pane3,point3,0);
					}
					else ;
				}
			}));
			ball3.setCycleCount(Timeline.INDEFINITE);
			ball3.play();
			
			ball4=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0) {//14:310.0,520.0,370.0,520.0,370.0,605.0,500.0,605.0,  15: 540.0,570.0,600.0,570.0
					if(point4.getCenterX()==370.0 && point4.getCenterY()<605.0) printTrack(3,pane3,point4,2);
					else if(point4.getCenterX()<=500.0 && point4.getCenterY()==605.0) {
						if(point4.getCenterX()==500.0) {point4.setCenterX(540.0);point4.setCenterY(570.0);}
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<=600.0 && point4.getCenterY()==570.0) {
						if(point4.getCenterX()==600.0) {pane2.getChildren().remove(2);point4.setCenterX(0.0);point4.setCenterY(0.0);}
						else printTrack(3,pane3,point4,0);
					}
				}
			}));
			ball4.setCycleCount(Timeline.INDEFINITE);
			ball4.play();
			
			break;
		 
		
		case "001000"://addi指令
			ball1=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()<=95.0 && point1.getCenterY()==480.0 ) {
					if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=130.0 && point1.getCenterY()==480.0) {
					if(point1.getCenterX()==130.0) {
						point1.setCenterX(310.0);
						point1.setCenterY(520.0);
					}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<370.0 && point1.getCenterY()==520.0) {
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterY()>450.0 && point1.getCenterX()==370.0)  {
					if(point1.getCenterY()==520.0) pane2.getChildren().add(point4);
					if(point1.getCenterY()==490.0) pane2.getChildren().add(point2);
					printTrack(0,pane3,point1,3);
				}
				else if(point1.getCenterX()<=600.0 && point1.getCenterY()==450.0) {
					if(point1.getCenterX()==600.0) {point1.setCenterX(780.0);point1.setCenterY(450.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=950.0&& point1.getCenterY()==450.0) {
					if(point1.getCenterX()==950.0) {point1.setCenterX(1050.0);point1.setCenterY(530.0);}
					else printTrack(0,pane3,point1,0); 
				}
				else if(point1.getCenterX()<1100.0 && point1.getCenterY()==530.0) printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()<700.0 && point1.getCenterX()==1100.0) printTrack(0,pane3,point1,2);
				else if(point1.getCenterX()<1350.0&&point1.getCenterX()>1090.0&& point1.getCenterY()==700.0) printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()>625.0 &&point1.getCenterY()<720.0&&point1.getCenterX()==1350.0 ) printTrack(0,pane3,point1,3);
				else if(point1.getCenterX()<=1380&& point1.getCenterX()>1340.0&&point1.getCenterY()==625.0) {
					if(point1.getCenterX()==1380.0) {point1.setCenterX(1420.0);point1.setCenterY(595.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<1440.0&&point1.getCenterX()>1380.0&& point1.getCenterY()==595.0)printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()<780.0 && point1.getCenterX()==1440.0) printTrack(0,pane3,point1,2);
				else if(point1.getCenterX()>560.0&& point1.getCenterY()==780.0) printTrack(0,pane3,point1,1);
				else if(point1.getCenterY()>595.0 && point1.getCenterX()==560.0) printTrack(0,pane3,point1,3);
				else if(point1.getCenterX()<=600.0&& point1.getCenterY()==595.0) {
					if(point1.getCenterX()==600.0) {pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
					else printTrack(0,pane3,point1,0);
				}
				else ;
			}));
			ball1.setCycleCount(Timeline.INDEFINITE);
			ball1.play();


			ball2=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {
					if(point2.getCenterX()<460.0 && point2.getCenterY()==490.0 ) {//370.0,490.0,460.0,490.0,,500.0,545.0   540.0,570.0,600.0,570.0
						 printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()==460.0 && point2.getCenterY()<545.0) {//460.0,545.0
						printTrack(1,pane3,point2,2);
					}
					else if(point2.getCenterX()<=500.0 && point2.getCenterY()==545.0) {
						if(point2.getCenterX()==500.0) {point2.setCenterX(540.0);point2.setCenterY(570.0);} 
						else printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()<=600.0 && point2.getCenterY()==570.0) {
						if(point2.getCenterX()==600.0) {pane2.getChildren().remove(3);point2.setCenterX(0.0);point2.setCenterY(0.0);}
						else  printTrack(1,pane3,point2,0);
					}
					else ;
				}
			}));
			ball2.setCycleCount(Timeline.INDEFINITE);
			ball2.play();

			ball3=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point1.getCenterX()>=95.0||point1.getCenterX()==0.0) {//70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0
					if(point3.getCenterX()==95.0 && point3.getCenterY()>165.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=190.0&&point3.getCenterX()>20.0 && point3.getCenterY()==165.0) {//260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0
						if(point3.getCenterX()==190.0) {point3.setCenterX(260.0);point3.setCenterY(190.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<820.0&&point3.getCenterX()>20.0 && point3.getCenterY()==190.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==820.0 && point3.getCenterY()>105.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=1220.0 &&point3.getCenterX()>20.0&& point3.getCenterY()==105.0) {//1260.0,200.0,1320.0,200.0
						if(point3.getCenterX()==1220.0) {point3.setCenterX(1260.0);point3.setCenterY(200.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<=1320.0&&point3.getCenterX()>20.0 && point3.getCenterY()==200.0) {//30.0,480.0,10.0,480.0,10.0,10.0,1380.0,10.0,1380.0,135.0,1360.0,135.0
						if(point3.getCenterX()==1320.0) {point3.setCenterX(1360.0);point3.setCenterY(135.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<1380.0&&point3.getCenterX()>20.0 && point3.getCenterY()==135.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==1380 && point3.getCenterY()>10.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()>10.0 && point3.getCenterY()==10.0) printTrack(2,pane3,point3,1);
					else if(point3.getCenterX()==10.0 && point3.getCenterY()<480.0)  printTrack(2,pane3,point3,2);
					else if(point3.getCenterX()<=30.0 && point3.getCenterY()==480.0)  {
						if(point3.getCenterX()==30.0) {pane2.getChildren().remove(0);point3.setCenterX(0.0);point3.setCenterY(0.0);}
						else printTrack(2,pane3,point3,0);
					}
					else ;
				}
			}));
			ball3.setCycleCount(Timeline.INDEFINITE);
			ball3.play();

			ball4=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0) {//310.0,520.0,370.0,520.0,370.0,690.0,830.0,690.0,830.0,615.0,860.0,615.0
					if(point4.getCenterX()==370.0 && point4.getCenterY()<690.0) printTrack(3,pane3,point4,2);
					else if(point4.getCenterX()<=730.0 && point4.getCenterY()==690.0) {//730,630,60,120
						if(point4.getCenterX()==730.0) point4.setCenterX(790.0);
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<830.0 && point4.getCenterY()==690.0) printTrack(3,pane3,point4,0);
					else if(point4.getCenterX()==830.0 && point4.getCenterY()>615.0) printTrack(3,pane3,point4,3);
					else if(point4.getCenterX()<=860.0 && point4.getCenterY()==615.0) {//900.0,575.0,950.0,575.0
						if(point4.getCenterX()==860.0) {point4.setCenterX(900.0);point4.setCenterY(575.0);}
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<=950.0 && point4.getCenterY()==575.0) {
						if(point4.getCenterX()==950.0){pane2.getChildren().remove(2);point4.setCenterX(0.0);point4.setCenterY(0.0);}
						else printTrack(3,pane3,point4,0);
					}
					else;
				}
			}));
			ball4.setCycleCount(Timeline.INDEFINITE);
			ball4.play();

			break;
			
		case "100011"://lw指令
			ball1=new Timeline(new KeyFrame(Duration.millis(3),e-> {				
				if(point1.getCenterX()<=95.0 && point1.getCenterY()==480.0 ) {
					if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=130.0 && point1.getCenterY()==480.0) {
					if(point1.getCenterX()==130.0) {
						point1.setCenterX(310.0);
						point1.setCenterY(520.0);
					}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<370.0 && point1.getCenterY()==520.0) {
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterY()>450.0 && point1.getCenterX()==370.0)  {
					if(point1.getCenterY()==520.0) pane2.getChildren().add(point4);
					if(point1.getCenterY()==490.0) pane2.getChildren().add(point2);
					printTrack(0,pane3,point1,3);
				}
				else if(point1.getCenterX()<=600.0 && point1.getCenterY()==450.0) {
					if(point1.getCenterX()==600.0) {point1.setCenterX(780.0);point1.setCenterY(450.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=950.0&& point1.getCenterY()==450.0) {
					if(point1.getCenterX()==950.0) {point1.setCenterX(1050.0);point1.setCenterY(530.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=1150.0 && point1.getCenterY()==530.0) {
					if(point1.getCenterX()==1150.0) point1.setCenterX(1330.0);
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=1380.0&&point1.getCenterY()==530.0) {
					if(point1.getCenterX()==1380.0) {point1.setCenterX(1420.0);point1.setCenterY(595.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<1440.0&&point1.getCenterX()>1380.0&& point1.getCenterY()==595.0)printTrack(0,pane3,point1,0);
				else if(point1.getCenterY()<780.0 && point1.getCenterX()==1440.0) printTrack(0,pane3,point1,2);
				else if(point1.getCenterX()>560.0&& point1.getCenterY()==780.0) printTrack(0,pane3,point1,1);
				else if(point1.getCenterY()>595.0 && point1.getCenterX()==560.0) printTrack(0,pane3,point1,3);
				else if(point1.getCenterX()<=600.0&& point1.getCenterY()==595.0) {
					if(point1.getCenterX()==600.0) {pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
					else printTrack(0,pane3,point1,0);
				}
				else ;
			}));
			ball1.setCycleCount(Timeline.INDEFINITE);
			ball1.play();

			ball2=new Timeline(new KeyFrame(Duration.millis(3),e-> {				
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {
					if(point2.getCenterX()<460.0 && point2.getCenterY()==490.0 ) {//370.0,490.0,460.0,490.0,,500.0,545.0   540.0,570.0,600.0,570.0
						printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()==460.0 && point2.getCenterY()<545.0) {//460.0,545.0
						printTrack(1,pane3,point2,2);
					}
					else if(point2.getCenterX()<=500.0 && point2.getCenterY()==545.0) {
						if(point2.getCenterX()==500.0) {point2.setCenterX(540.0);point2.setCenterY(570.0);} 
						else printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()<=600.0 && point2.getCenterY()==570.0) {
						if(point2.getCenterX()==600.0) {pane2.getChildren().remove(3);point2.setCenterX(0.0);point2.setCenterY(0.0);}
						else  printTrack(1,pane3,point2,0);
					}
					else ;
				}
			}));
			ball2.setCycleCount(Timeline.INDEFINITE);
			ball2.play();

			ball3=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point1.getCenterX()>=95.0||point1.getCenterX()==0.0) {//70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0
					if(point3.getCenterX()==95.0 && point3.getCenterY()>165.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=190.0&&point3.getCenterX()>20.0 && point3.getCenterY()==165.0) {//260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0
						if(point3.getCenterX()==190.0) {point3.setCenterX(260.0);point3.setCenterY(190.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<820.0&&point3.getCenterX()>20.0 && point3.getCenterY()==190.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==820.0 && point3.getCenterY()>105.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=1220.0 &&point3.getCenterX()>20.0&& point3.getCenterY()==105.0) {//1260.0,200.0,1320.0,200.0
						if(point3.getCenterX()==1220.0) {point3.setCenterX(1260.0);point3.setCenterY(200.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<=1320.0&&point3.getCenterX()>20.0 && point3.getCenterY()==200.0) {//30.0,480.0,10.0,480.0,10.0,10.0,1380.0,10.0,1380.0,135.0,1360.0,135.0
						if(point3.getCenterX()==1320.0) {point3.setCenterX(1360.0);point3.setCenterY(135.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<1380.0&&point3.getCenterX()>20.0 && point3.getCenterY()==135.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==1380 && point3.getCenterY()>10.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()>10.0 && point3.getCenterY()==10.0) printTrack(2,pane3,point3,1);
					else if(point3.getCenterX()==10.0 && point3.getCenterY()<480.0)  printTrack(2,pane3,point3,2);
					else if(point3.getCenterX()<=30.0 && point3.getCenterY()==480.0)  {
						if(point3.getCenterX()==30.0) {pane2.getChildren().remove(0);point3.setCenterX(0.0);point3.setCenterY(0.0);}
						else printTrack(2,pane3,point3,0);
					}
					else ;
				}
			}));
			ball3.setCycleCount(Timeline.INDEFINITE);
			ball3.play();

			ball4=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0) {//310.0,520.0,370.0,520.0,370.0,690.0,830.0,690.0,830.0,615.0,860.0,615.0
					if(point4.getCenterX()==370.0 && point4.getCenterY()<690.0) printTrack(3,pane3,point4,2);
					else if(point4.getCenterX()<=730.0 && point4.getCenterY()==690.0) {//730,630,60,120
						if(point4.getCenterX()==730.0) point4.setCenterX(790.0);
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<830.0 && point4.getCenterY()==690.0) printTrack(3,pane3,point4,0);
					else if(point4.getCenterX()==830.0 && point4.getCenterY()>615.0) printTrack(3,pane3,point4,3);
					else if(point4.getCenterX()<=860.0 && point4.getCenterY()==615.0) {//900.0,575.0,950.0,575.0
						if(point4.getCenterX()==860.0) {point4.setCenterX(900.0);point4.setCenterY(575.0);}
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<=950.0 && point4.getCenterY()==575.0) {
						if(point4.getCenterX()==950.0){pane2.getChildren().remove(2);point4.setCenterX(0.0);point4.setCenterY(0.0);}
						else printTrack(3,pane3,point4,0);
					}
					else;
				}
			}));
			ball4.setCycleCount(Timeline.INDEFINITE);
			ball4.play();

			break;
				
		case "101011"://sw指令
			ball1=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()<=95.0 && point1.getCenterY()==480.0 ) {
					if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=130.0 && point1.getCenterY()==480.0) {
					if(point1.getCenterX()==130.0) {
						point1.setCenterX(310.0);
						point1.setCenterY(520.0);
					}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<370.0 && point1.getCenterY()==520.0) {
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterY()>450.0 && point1.getCenterX()==370.0)  {
					if(point1.getCenterY()==520.0) pane2.getChildren().add(point4);
					if(point1.getCenterY()==490.0) pane2.getChildren().add(point2);
					printTrack(0,pane3,point1,3);
				}
				else if(point1.getCenterX()<=600.0 && point1.getCenterY()==450.0) {
					if(point1.getCenterX()==600.0) {point1.setCenterX(780.0);point1.setCenterY(450.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=950.0&& point1.getCenterY()==450.0) {
					if(point1.getCenterX()==950.0) {point1.setCenterX(1050.0);point1.setCenterY(530.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=1150.0 && point1.getCenterY()==530.0) {
					if(point1.getCenterX()==1150.0){pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
					else printTrack(0,pane3,point1,0);
				}
				else ;
			}));
			ball1.setCycleCount(Timeline.INDEFINITE);
			ball1.play();


		    ball2=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {
					if(point2.getCenterX()<=600.0 && point2.getCenterY()==490.0 ) {//780.0,555.0,860.0,555.0
						if(point2.getCenterX()==600.0) {point2.setCenterX(780.0);point2.setCenterY(555.0);}
						else printTrack(1,pane3,point2,0);
					}					
					else if(point2.getCenterX()<800.0 && point2.getCenterY()==555.0) printTrack(1,pane3,point2,0);
					else if(point2.getCenterX()==800.0 && point2.getCenterY()<650.0) printTrack(1,pane3,point2,2);
					else if(point2.getCenterX()<=1150.0 && point2.getCenterY()==650.0) {
						if(point2.getCenterX()==1150.0) {pane2.getChildren().remove(1);point2.setCenterX(0.0);point2.setCenterY(0.0);}
						else printTrack(1,pane3,point2,0);
					}
					else ;
				}
			}));
			ball2.setCycleCount(Timeline.INDEFINITE);
			ball2.play();

			ball3=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point1.getCenterX()>=95.0||point1.getCenterX()==0.0) {//70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0
					if(point3.getCenterX()==95.0 && point3.getCenterY()>165.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=190.0&&point3.getCenterX()>20.0 && point3.getCenterY()==165.0) {//260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0
						if(point3.getCenterX()==190.0) {point3.setCenterX(260.0);point3.setCenterY(190.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<820.0&&point3.getCenterX()>20.0 && point3.getCenterY()==190.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==820.0 && point3.getCenterY()>105.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=1220.0 &&point3.getCenterX()>20.0&& point3.getCenterY()==105.0) {//1260.0,200.0,1320.0,200.0
						if(point3.getCenterX()==1220.0) {point3.setCenterX(1260.0);point3.setCenterY(200.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<=1320.0&&point3.getCenterX()>20.0 && point3.getCenterY()==200.0) {//30.0,480.0,10.0,480.0,10.0,10.0,1380.0,10.0,1380.0,135.0,1360.0,135.0
						if(point3.getCenterX()==1320.0) {point3.setCenterX(1360.0);point3.setCenterY(135.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<1380.0&&point3.getCenterX()>20.0 && point3.getCenterY()==135.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==1380 && point3.getCenterY()>10.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()>10.0 && point3.getCenterY()==10.0) printTrack(2,pane3,point3,1);
					else if(point3.getCenterX()==10.0 && point3.getCenterY()<480.0)  printTrack(2,pane3,point3,2);
					else if(point3.getCenterX()<=30.0 && point3.getCenterY()==480.0)  {
						if(point3.getCenterX()==30.0) {pane2.getChildren().remove(0);point3.setCenterX(0.0);point3.setCenterY(0.0);}
						else printTrack(2,pane3,point3,0);
					}
					else ;
				}
			}));
			ball3.setCycleCount(Timeline.INDEFINITE);
			ball3.play();

			ball4=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {//310.0,520.0,370.0,520.0,370.0,690.0,830.0,690.0,830.0,615.0,860.0,615.0
					if(point4.getCenterX()==370.0 && point4.getCenterY()<690.0) printTrack(3,pane3,point4,2);
					else if(point4.getCenterX()<=730.0 && point4.getCenterY()==690.0) {//730,630,60,120
						if(point4.getCenterX()==730.0) point4.setCenterX(790.0);
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<830.0 && point4.getCenterY()==690.0) printTrack(3,pane3,point4,0);
					else if(point4.getCenterX()==830.0 && point4.getCenterY()>615.0) printTrack(3,pane3,point4,3);
					else if(point4.getCenterX()<=860.0 && point4.getCenterY()==615.0) {//900.0,575.0,950.0,575.0
						if(point4.getCenterX()==860.0) {point4.setCenterX(900.0);point4.setCenterY(575.0);}
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<=950.0 && point4.getCenterY()==575.0) {
						if(point4.getCenterX()==950.0){pane2.getChildren().remove(1);point4.setCenterX(0.0);point4.setCenterY(0.0);}
						else printTrack(3,pane3,point4,0);
					}
					else;
				}
			}));
			ball4.setCycleCount(Timeline.INDEFINITE);
			ball4.play();
				

			break;
			
		case "000100"://beq指令
			ball1=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()<=95.0 && point1.getCenterY()==480.0 ) {
					if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=130.0 && point1.getCenterY()==480.0) {
					if(point1.getCenterX()==130.0) {
						point1.setCenterX(310.0);
						point1.setCenterY(520.0);
					}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<370.0 && point1.getCenterY()==520.0) {
					printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterY()>450.0 && point1.getCenterX()==370.0)  {
					if(point1.getCenterY()==520.0) pane2.getChildren().add(point4);
					if(point1.getCenterY()==490.0) pane2.getChildren().add(point2);
					printTrack(0,pane3,point1,3);
				}
				else if(point1.getCenterX()<=600.0 && point1.getCenterY()==450.0) {
					if(point1.getCenterX()==600.0) {point1.setCenterX(780.0);point1.setCenterY(450.0);}
					else printTrack(0,pane3,point1,0);
				}
				else if(point1.getCenterX()<=950.0&& point1.getCenterY()==450.0) {
					if(point1.getCenterX()==950.0) {pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
					else printTrack(0,pane3,point1,0); 
				}
				else ;
			}));
			ball1.setCycleCount(Timeline.INDEFINITE);
			ball1.play();
			
			ball2=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {
					if(point2.getCenterX()<=600.0 && point2.getCenterY()==490.0 ) {//780.0,555.0,860.0,555.0
						if(point2.getCenterX()==600.0) {point2.setCenterX(780.0);point2.setCenterY(555.0);}
						else printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()<=860.0 && point2.getCenterY()==555.0) {//900.0,575.0,950.0,575.0
						if(point2.getCenterX()==860.0) {point2.setCenterX(900.0);point2.setCenterY(575.0);}
						else printTrack(1,pane3,point2,0);
					}
					else if(point2.getCenterX()<=950.0 && point2.getCenterY()==575.0) {//900.0,575.0,950.0,575.0
						if(point2.getCenterX()==950.0) {
								pane2.getChildren().remove(3);point2.setCenterX(0.0);point2.setCenterY(0.0);
						}
						else printTrack(1,pane3,point2,0);
					}
					else ;
				}
			}));
			ball2.setCycleCount(Timeline.INDEFINITE);
			ball2.play();


			ball3=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point1.getCenterX()>=95.0||point1.getCenterX()==0.0) {//70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0
					if(point3.getCenterX()==95.0 && point3.getCenterY()>165.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()<=190.0&&point3.getCenterX()>40.0 && point3.getCenterY()==165.0) {//260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0
						if(point3.getCenterX()==190.0) {point3.setCenterX(260.0);point3.setCenterY(190.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<820.0&&point3.getCenterX()>20.0 && point3.getCenterY()==190.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==820.0 && point3.getCenterY()>105.0) {
						if(point3.getCenterY()==145.0) pane2.getChildren().add(point5);
						printTrack(2,pane3,point3,3);
					}
					else if(point3.getCenterX()<=1220.0 &&point3.getCenterX()>100.0&& point3.getCenterY()==105.0) {//1260.0,200.0,1320.0,200.0
						if(point3.getCenterX()==1220.0) {point3.setCenterX(1260.0);point3.setCenterY(200.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<=1320.0&&point3.getCenterX()>100.0 && point3.getCenterY()==200.0) {//30.0,480.0,10.0,480.0,10.0,10.0,1380.0,10.0,1380.0,135.0,1360.0,135.0
						if(point3.getCenterX()==1320.0) {point3.setCenterX(1360.0);point3.setCenterY(135.0);}
						else printTrack(2,pane3,point3,0);
					}
					else if(point3.getCenterX()<1380.0&&point3.getCenterX()>100.0 && point3.getCenterY()==135.0) printTrack(2,pane3,point3,0);
					else if(point3.getCenterX()==1380 && point3.getCenterY()>10.0) printTrack(2,pane3,point3,3);
					else if(point3.getCenterX()>10.0 && point3.getCenterY()==10.0) printTrack(2,pane3,point3,1);
					else if(point3.getCenterX()==10.0 && point3.getCenterY()<480.0)  printTrack(2,pane3,point3,2);
					else if(point3.getCenterX()<=30.0 && point3.getCenterY()==480.0)  {
						if(point3.getCenterX()==30.0) {pane2.getChildren().remove(0);point3.setCenterX(0.0);point3.setCenterY(0.0);}
						else printTrack(2,pane3,point3,0);
					}
					else ;
				}
			}));
			ball3.setCycleCount(Timeline.INDEFINITE);
			ball3.play();

			ball4=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()>=370.0||point1.getCenterX()==0.0) {//310.0,520.0,370.0,520.0,370.0,690.0,830.0,690.0,830.0,615.0,860.0,615.0
					if(point4.getCenterX()==370.0 && point4.getCenterY()<690.0) printTrack(3,pane3,point4,2);
					else if(point4.getCenterX()<=730.0 && point4.getCenterY()==690.0) {//730,630,60,120
						if(point4.getCenterX()==730.0) point4.setCenterX(790.0);
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<830.0 && point4.getCenterY()==690.0) printTrack(3,pane3,point4,0);
					else if(point4.getCenterX()==830.0 && point4.getCenterY()>255.0) printTrack(3,pane3,point4,3);
					
					else if(point4.getCenterX()<=860.0 && point4.getCenterY()==255.0) {//900.0,575.0,950.0,575.0
						if(point4.getCenterX()==860.0) point4.setCenterX(905.0);
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<=950.0 && point4.getCenterY()==255.0) {
						if(point4.getCenterX()==950.0){point4.setCenterX(1050.0);point4.setCenterY(200.0);}
						else printTrack(3,pane3,point4,0);
					}
					else if(point4.getCenterX()<=1220.0 && point4.getCenterY()==200.0) {
						if(point4.getCenterX()==1220.0){pane2.getChildren().remove(1);point4.setCenterX(0.0);point4.setCenterY(0.0);}
						else printTrack(3,pane3,point4,0);
					}
					else;
				}
			}));
			ball4.setCycleCount(Timeline.INDEFINITE);
			ball4.play();

			ball5=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point3.getCenterX()>=820.0) {
					if(point5.getCenterX()==950.0) {pane2.getChildren().remove(2);point5.setCenterX(0.0);point5.setCenterY(0.0);}
					else if(point5.getCenterY()==145.0) printTrack(4,pane3,point5,0);
					else;
				}
				else;
			}));
			ball5.setCycleCount(Timeline.INDEFINITE);
			ball5.play();

			break;
		
		case "000010"://jump指令
			ball1=new Timeline(new KeyFrame(Duration.millis(3),e-> {
				if(point1.getCenterX()<=130.0 &&point1.getCenterX()>50.0 && point1.getCenterY()==480.0) {
					if(point1.getCenterX()==95.0) pane2.getChildren().add(point3);
					if(point1.getCenterX()==130.0) {
						point1.setCenterX(310.0);
						point1.setCenterY(520.0);
					}
					else printTrack(4,pane3,point1,0);
				}
				else if(point1.getCenterX()<330.0 && point1.getCenterY()==520.0) printTrack(4,pane3,point1,0);
				else if(point1.getCenterX()==330.0 && point1.getCenterY()>65.0) printTrack(4,pane3,point1,3);
				else if(point1.getCenterX()<=460.0 &&point1.getCenterX()>320.0&& point1.getCenterY()==65.0) {//shift_left1:460,20,45,90
					if(point1.getCenterX()==460.0) point1.setCenterX(505.0);
					else printTrack(4,pane3,point1,0);	
				}
				else if(point1.getCenterX()<1280.0  && point1.getCenterX()>500.0 &&point1.getCenterY()==65.0)printTrack(4,pane3,point1,0);
				else if(point1.getCenterX()==1280.0 && point1.getCenterY()<105.0&&point1.getCenterY()>20.0) printTrack(4,pane3,point1,2);
				else if(point1.getCenterX()<=1320.0 && point1.getCenterX()>1270.0 &&point1.getCenterY()==105.0) {
					if(point1.getCenterX()==1320.0) {point1.setCenterX(1360.0);point1.setCenterY(135.0);}
					else printTrack(4,pane3,point1,0);
				}
				else if(point1.getCenterX()<1380.0&&point1.getCenterX()>20.0 && point1.getCenterY()==135.0) printTrack(4,pane3,point1,0);
				else if(point1.getCenterX()==1380 && point1.getCenterY()>10.0) printTrack(4,pane3,point1,3);
				else if(point1.getCenterX()>10.0 && point1.getCenterY()==10.0)printTrack(4,pane3,point1,1);
				else if(point1.getCenterX()==10.0 && point1.getCenterY()<480.0)  printTrack(4,pane3,point1,2);
				else if(point1.getCenterX()<=30.0 && point1.getCenterY()==480.0)  {
					if(point1.getCenterX()==30.0) {pane2.getChildren().remove(0);point1.setCenterX(0.0);point1.setCenterY(0.0);}
					else printTrack(4,pane3,point1,0);
				}
				else ;
			}));
			ball1.setCycleCount(Timeline.INDEFINITE);
			ball1.play();
			

			//line6:260.0,190.0,360.0,190.0,360.0,120.0,600.0,120.0,600.0,65.0
			ball3=new Timeline(new KeyFrame(Duration.millis(3),e->{
				if(point1.getCenterX()>=95.0||point1.getCenterX()==0.0) {//70.0,480.0,95.0,480.0,95.0,165.0,190.0,165.0
					if(point3.getCenterX()==95.0 && point3.getCenterY()>165.0) printTrack(0,pane3,point3,3);
					else if(point3.getCenterX()<=190.0&&point3.getCenterX()>20.0 && point3.getCenterY()==165.0) {//260.0,190.0,820.0,190.0,820.0,105.0,1220.0,105.0
						if(point3.getCenterX()==190.0) {point3.setCenterX(260.0);point3.setCenterY(190.0);}
						else printTrack(0,pane3,point3,0);
					}
					else if(point3.getCenterX()<360.0&&point3.getCenterY()==190.0)printTrack(0,pane3,point3,0);
					else if(point3.getCenterX()==360.0&&point3.getCenterY()>120.0) printTrack(0,pane3,point3,3);
					else if(point3.getCenterX()<600.0&&point3.getCenterY()==120.0) printTrack(0,pane3,point3,0);
					else if(point3.getCenterX()==600.0&&point3.getCenterY()>=65.0) {
						if(point3.getCenterY()==65.0) {pane2.getChildren().remove(1);point3.setCenterX(0.0);point3.setCenterY(0.0);}
						printTrack(0,pane3,point3,3);
					}
					else ;
				}
			}));
			ball3.setCycleCount(Timeline.INDEFINITE);
			ball3.play();
			break;
		}
		

		
		Scene secondscene = new Scene(pane,1500,850);
		Stage SecondStage = new Stage();
		SecondStage.setWidth(1502);
		SecondStage.setHeight(852);
		SecondStage.setResizable(false);
		SecondStage.setTitle("CPU data path");
		SecondStage.setScene(secondscene);
		SecondStage.show();
	}
	
	public void printTrack(int i,Pane pane,Circle point,int flag) {
		double former_x=point.getCenterX();
		double former_y=point.getCenterY();
		if(flag==0) point.setCenterX(point.getCenterX()+1.0);
		else if(flag==1)point.setCenterX(point.getCenterX()-1.0);
		else if(flag==2)point.setCenterY(point.getCenterY()+1.0);
		else if(flag==3)point.setCenterY(point.getCenterY()-1.0);
		Line l=new Line(point.getCenterX(),point.getCenterY(),former_x,former_y);
		l.setStrokeWidth(10);
		l.setFill(Color.GREEN);
		l.setStroke(Color.GREEN);
		pane.getChildren().add(l);
	}
	
	public static void main(String[] args) {
		Application.launch(args);
	}
}