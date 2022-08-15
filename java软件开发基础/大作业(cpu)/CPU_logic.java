package cpu;
import java.io.*;
import java.math.BigInteger;
//import java.util.Scanner;
public class CPU_logic {
	public String PC,PC_pre;
	public String[] Instruction_memory = new String[64];
	public String[] Data_memory = new String[64];
	public String[] Registers = new String[32];
	public String instruction;
	int rs,rt,rd;
	String addr;
	public CPU_logic() {
		instruction=null;
		PC="00000000000000000000000000000000";
		for(int i=0;i<32;i++) {
			Registers[i]="00000000000000000000000000000000";
		}
		for(int i=0;i<64;i++) {
			Instruction_memory[i]="00000000000000000000000000000000";
			Data_memory[i]="00000000000000000000000000000000";
		}
		rs=0;rt=0;rd=0;addr=null;
	}
	
	public boolean load_file(File readFile,int flag) {//�ļ���ȡ��flagΪ0����ȡָ�������ļ���flagΪ1����ȡ�ڴ������ļ� 
	    BufferedReader reader = null;
	    int radix=0;//���ݽ���
	    try{
	        reader = new BufferedReader(new FileReader(readFile));
	        String tempString = null;
	        int line = 1;
	        while ((tempString = reader.readLine()) != null){//BufferedReader��readLine()������ʵ�ְ��ж�ȡ
	        	if(line==1) {//��һ����ʾ���ݵĽ���,֧��16,2����
	        		tempString=tempString.trim();
		            if (tempString.indexOf("16")!=-1) {
		            	radix=16;
		            }
		            else if(tempString.indexOf("2")!=-1) {
		            	radix=2;
		            }
	        	}
	        	else if(line!=2){//�����еĴ���:ת��Ϊ�������ַ���
	        		if(radix==2) {
	        			if(flag==0) Instruction_memory[line-3]=tempString;
	        			else Data_memory[line-3]=tempString;
	        		}
	        		else if(radix==16) {
	        			if(flag==0) Instruction_memory[line-3]= hexString2binaryString(tempString);
	        			else Data_memory[line-3]=hexString2binaryString(tempString);
	        		}
	        		
	        	}
	            line++;
	        }
	        reader.close();
	    }catch(IOException e){
	        e.printStackTrace();
	    }
	    
	    return true;    
	 }
	
	
	public static String hexString2binaryString(String hexString) {//16�����ַ���ת��Ϊ�������ַ���********OK
		if (hexString == null || hexString.length() % 2 != 0) return null;
		String bString = "", tmp;
		for (int i = 0; i < hexString.length(); i++) {
			tmp = "0000" + Integer.toBinaryString(Integer.parseInt(hexString.substring(i, i + 1), 16));
			bString += tmp.substring(tmp.length() - 4);
		}
		return bString;
	}
	
	public boolean run_one() {
		instruction = Instruction_memory[Integer.parseUnsignedInt(binaryStringToDecimalString("00".concat(PC.substring(0,30))))];//ȡָ
		PC_pre=PC;
		PC=binaryStringAddbinaryString(PC,"00000000000000000000000000000100");//PC+4
		switch(instruction.substring(0,6)) {
		case "000000"://addָ��
			rs = Integer.parseUnsignedInt(instruction.substring(6,11),2);
			rt = Integer.parseUnsignedInt(instruction.substring(11,16),2);
			rd = Integer.parseUnsignedInt(instruction.substring(16,21),2);
			if(rd==0) break;
			Registers[rd]=binaryStringAddbinaryString(Registers[rs],Registers[rt]);
			break;
		case "001000"://addiָ��
			rs = Integer.parseUnsignedInt(instruction.substring(6,11),2);
			rt = Integer.parseUnsignedInt(instruction.substring(11,16),2);
			if(rt==0) break;
			Registers[rt]=binaryStringAddbinaryString(Registers[rs],sign_extend(instruction.substring(16)));
			break;
		case "100011"://lwָ��
			rs = Integer.parseUnsignedInt(instruction.substring(6,11),2);
			rt = Integer.parseUnsignedInt(instruction.substring(11,16),2);
			if(rt==0) break;
			addr=binaryStringAddbinaryString(Registers[rs],sign_extend(instruction.substring(16)));
			Registers[rt]=Data_memory[Integer.parseUnsignedInt(binaryStringToDecimalString("00".concat(addr.substring(0,30))))];
			break;
		case "101011"://swָ��
			rs = Integer.parseUnsignedInt(instruction.substring(6,11),2);
			rt = Integer.parseUnsignedInt(instruction.substring(11,16),2);
			addr=binaryStringAddbinaryString(Registers[rs],sign_extend(instruction.substring(16)));
			Data_memory[Integer.parseUnsignedInt(binaryStringToDecimalString("00".concat(addr.substring(0,30))))]=Registers[rt];
			break;
		case "000100"://beqָ��
			rs = Integer.parseUnsignedInt(instruction.substring(6,11),2);
			rt = Integer.parseUnsignedInt(instruction.substring(11,16),2);
			if(Registers[rs].compareTo(Registers[rt])==0) {//rs==rt
				PC=binaryStringAddbinaryString(PC,Shift_left(sign_extend(instruction.substring(16)),32,2));
			}
			break;
		case "000010"://jumpָ��
			PC=PC.substring(0,4).concat(Shift_left("00".concat(instruction.substring(6)),28,2));
			break;
		}
		return true;
		
	}
	
	public  String sign_extend(String a) {//��16λ�ַ����з�����չΪ32λ
		String neg="1111111111111111";
		String pos="0000000000000000";
		if(a.substring(0, 1)=="1") return neg.concat(a);
		else  return pos.concat(a);
	}
	
	public  String Shift_left(String a ,int m,int n) {//������Ϊm�Ķ�����a������nλ������mλ�ַ���
		String tmp = "0";
		for(int i=1;i<m;i++) {
			tmp=tmp.concat("0");
		}
		StringBuilder sb=new StringBuilder(tmp);
		sb.replace(0,m-n,a.substring(n,m));
		tmp=sb.toString();
		return tmp;
	}
	
	public  String binaryStringAddbinaryString(String a,String b) {//������32λ�������ַ����������������
		String tmp="00000000000000000000000000000000";
		StringBuilder temp=new StringBuilder(tmp);
		int carry_bit=0;//��λ
		for(int i=31;i>=0;i--) {
			if(a.substring(i,i+1).equals("1") && b.substring(i,i+1).equals("1") && carry_bit==0)carry_bit=1;
			else if(a.substring(i,i+1).equals("1") && b.substring(i,i+1).equals("0") && carry_bit==1)carry_bit=1;
			else if(a.substring(i,i+1).equals("0")&&b.substring(i,i+1).equals("1")&&carry_bit==1) carry_bit=1;
			
			else if(a.substring(i,i+1).equals("1")&&b.substring(i,i+1).equals("1")&&carry_bit==1) {
				carry_bit=1;
				temp.replace(i, i+1, "1");
			}
			else if(a.substring(i,i+1).equals("0")&&b.substring(i,i+1).equals("0")&&carry_bit==0) {
				carry_bit=0;
			}
			else {
				carry_bit=0;
				temp.replace(i, i+1, "1");
			}
		}
		tmp=temp.toString();
		return tmp;
	}
	
	public  String binaryStringToDecimalString(String a) {//32λ�������ַ���ת��Ϊʮ�����ַ���
		int value=0;
		int flag=0;
		if(a.substring(0,1).equals("1")) {//��������ʾ -x ��ʽ
			flag=1;
			StringBuilder sb=new StringBuilder(a);
			for(int i=0;i<a.length();i++) {//ȡ��
				if(a.substring(i,i+1).equals("1")) sb.replace(i,i+1,"0");
				else sb.replace(i, i+1, "1");
			}
			a=sb.toString();
			binaryStringAddbinaryString(a,"00000000000000000000000000000001");
		}
		for(int i=0;i<32;i++) {
			if(a.substring(i, i+1).equals("1")) {
				value+=Math.pow(2,31-i);
			}
		}
		return (flag==1)? "-".concat(Integer.toString(value)):Integer.toString(value);
	}
	public  String DecimalStringTobinaryString(String a) {//ʮ�����ַ���ת��Ϊ32λ�������ַ���******����
		BigInteger tmp = new BigInteger(String.valueOf(a)); //ת����BigInteger����
		String binary= tmp.toString(2); //����2ָ������ת����X���ƣ�Ĭ��10����
		int length=binary.length();
		if (binary.substring(0,1).equals("-")) {//����Ҫת�����䲹��
			binary=binary.substring(1);
			StringBuilder sb=new StringBuilder(binary);
			for(int i=0;i<binary.length();i++) {
				if(binary.substring(i,i+1).equals("1")) sb.replace(i,i+1,"0");
				else sb.replace(i, i+1, "1");
			}
			binary=sb.toString();
			for(int i=0;i<32-(length-1);i++) {
				binary="1".concat(binary);
			}
			return binaryStringAddbinaryString(binary,"00000000000000000000000000000001");
		}
		else {//�Ǹ���
			for(int i=0;i<32-length;i++) {
				binary="0".concat(binary);
			}
			return binary;
		}
	}
	
	public int Reg_Parse(String Regname) {//��$t0-$t7��8-15��,$s0-s7��16-23����ֵ��Ӧת��Ϊ�Ĵ������
		int index=8;
		if(Regname.substring(1,2).equals("s")) index+=8;
		index+=Integer.valueOf(Regname.substring(2,3)).intValue();
		return index;
	}
}