APP_ABI := armeabi
#all

APP_PLATFORM := android-8
 
# system - ʹ��Ĭ����С��C++���п⣬�������ɵ�Ӧ�����С���ڴ�ռ��С�������ֹ��ܽ��޷�֧��
# stlport_static - ʹ��STLport��Ϊ��̬�⣬������Android�����������Ƽ���
# stlport_shared - STLport ��Ϊ��̬�⣬������ܲ��������ԺͲ��ֵͰ汾��Android�̼���Ŀǰ���Ƽ�ʹ�á�
# gnustl_static - ʹ�� GNU libstdc++ ��Ϊ��̬��
# ʹ��STLport��Ϊ��̬�⣬������Android�����������Ƽ���
#APP_STL := stlport_static
#APP_STL := gnustl_static
APP_STL := gnustl_static

# �����쳣֧�֣�  error: exception handling disabled, use -fexceptions to enable

APP_CPPFLAGS += -fexceptions
APP_CPPFLAGS += -frtti