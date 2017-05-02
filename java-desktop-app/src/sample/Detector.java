package sample;

/**
 * Created by Piotr Szubert on 24.04.2017.
 */
public class Detector {
        private String value;
        private String time;

        public Detector(String value, String time) {
            this.value = value;
            this.time = time;
        }

        public String getValue() {
            return value;
        }

        public String getTime() {
            return time;
        }
}
